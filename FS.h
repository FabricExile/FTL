/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/Path.h>
#include <FTL/CStrRef.h>

#include <string>
#include <string.h>
#include <vector>
#if defined(FTL_PLATFORM_POSIX)
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <pwd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>
# include <stdio.h>
#elif defined(FTL_PLATFORM_WINDOWS)
# include <Windows.h>
#else
# error "Unsupport FTL_PLATFORM_..."
#endif

FTL_NAMESPACE_BEGIN

struct FSStatInfo
{
  enum Type
  {
    File,
    Dir,
    Other
  } type;
};

inline bool FSStat(
  char const *pathCStr,
  FSStatInfo &statInfo
  )
{
  if ( !pathCStr || !*pathCStr )
    pathCStr = ".";

#if defined(FTL_PLATFORM_POSIX)
  struct stat st;
  if ( ::stat( pathCStr, &st ) == -1 )
    return false;

  if ( S_ISREG( st.st_mode ) )
    statInfo.type = FSStatInfo::File;
  else if ( S_ISDIR( st.st_mode ) )
    statInfo.type = FSStatInfo::Dir;
  else
    statInfo.type = FSStatInfo::Other;

  return true;
#elif defined(FTL_PLATFORM_WINDOWS)

  DWORD fa = ::GetFileAttributesA( pathCStr );
  if ( fa == INVALID_FILE_ATTRIBUTES )
    return false;

  if ( fa & FILE_ATTRIBUTE_DIRECTORY )
    statInfo.type = FSStatInfo::Dir;
  else if ( fa & FILE_ATTRIBUTE_NORMAL || fa & FILE_ATTRIBUTE_ARCHIVE )
    statInfo.type = FSStatInfo::File;
  else
    statInfo.type = FSStatInfo::Other;

  return true;
#endif
}

inline bool FSStat(
  std::string const &pathStdString,
  FSStatInfo &statInfo
  )
{
  return FSStat( pathStdString.c_str(), statInfo );
}

inline bool FSStat(
  StrRef pathStr,
  FSStatInfo &statInfo
  )
{
  char *pathCStr = (char *)alloca( pathStr.size() + 1 );
  memcpy( pathCStr, pathStr.data(), pathStr.size() );
  pathCStr[pathStr.size()] = '\0';
  return FSStat( pathCStr, statInfo );
}  

inline bool FSExists( char const *pathCStr )
{
  FSStatInfo statInfo;
  return FSStat( pathCStr, statInfo );
}

inline bool FSExists( std::string const &pathStdString )
{
  return FSExists( pathStdString.c_str() );
}

inline bool FSExists( StrRef const &pathStr )
{
  char *pathCStr = (char *)alloca( pathStr.size() + 1 );
  memcpy( pathCStr, pathStr.data(), pathStr.size() );
  pathCStr[pathStr.size()] = '\0';
  return FSExists( pathCStr );
}

inline bool FSIsFile( char const *pathCStr )
{
  FSStatInfo statInfo;
  return FSStat( pathCStr, statInfo )
    && statInfo.type == FSStatInfo::File;
}

inline bool FSIsFile( std::string const &path )
{
  return FSIsFile( path.c_str() );
}

inline bool FSIsFile( StrRef const &pathStr )
{
  char *pathCStr = (char *)alloca( pathStr.size() + 1 );
  memcpy( pathCStr, pathStr.data(), pathStr.size() );
  pathCStr[pathStr.size()] = '\0';
  return FSIsFile( pathCStr );
}

inline bool FSIsDir( char const *pathCStr )
{
  FSStatInfo statInfo;
  return FSStat( pathCStr, statInfo )
    && statInfo.type == FSStatInfo::Dir;
}

inline bool FSIsDir( std::string const &path )
{
  return FSIsDir( path.c_str() );
}

inline bool FSIsDir( StrRef const &pathStr )
{
  char *pathCStr = (char *)alloca( pathStr.size() + 1 );
  memcpy( pathCStr, pathStr.data(), pathStr.size() );
  pathCStr[pathStr.size()] = '\0';
  return FSIsDir( pathCStr );
}

inline bool FSMkDir( char const *pathCStr )
{
  FSStatInfo statInfo;
  if ( FSStat( pathCStr, statInfo ) )
    return statInfo.type == FSStatInfo::Dir;

  if ( !pathCStr || !*pathCStr )
    return true;
  
#if defined(FTL_PLATFORM_POSIX)
  return ::mkdir( pathCStr, 0777 ) == 0;
#elif defined(FTL_PLATFORM_WINDOWS)
  return ::CreateDirectoryA( pathCStr, NULL );
#endif
}

inline bool FSMkDir( std::string const &path )
{
  return FSMkDir( path.c_str() );
}

inline bool FSMkDir( StrRef const &pathStr )
{
  char *pathCStr = (char *)alloca( pathStr.size() + 1 );
  memcpy( pathCStr, pathStr.data(), pathStr.size() );
  pathCStr[pathStr.size()] = '\0';
  return FSMkDir( pathCStr );
}

inline bool FSMkDirRecursive( StrRef pathStr )
{
  if ( pathStr.empty() )
    return true;

  std::pair<StrRef, StrRef> pathSplit = PathSplit( pathStr );

  if ( !pathSplit.first.empty() )
  {
    if ( !FSMkDirRecursive( pathSplit.first ) )
      return false;
  }
  return FSMkDir( pathStr );
}

inline bool FSDirAppendEntries(
  char const *pathCStr,
  std::vector<std::string> &entries
  )
{
  if ( !pathCStr || !*pathCStr )
    pathCStr = ".";

#if defined(FTL_PLATFORM_POSIX)
  DIR *dir = ::opendir( pathCStr );
  if ( !dir )
    return false;
  for (;;)
  {
    struct dirent *de = ::readdir( dir );
    if ( !de )
      break;
    
    // [pzion 20130723] Skip . and ..
    char const *entryCStr = de->d_name;
    if ( entryCStr[0] == '.'
      && (entryCStr[1] == '\0'
        || (entryCStr[1] == '.' && entryCStr[2] == '\0')
        )
      )
      continue;

    entries.push_back( entryCStr );
  }
  ::closedir( dir );
#elif defined(FTL_OS_WINDOWS)
  std::string searchGlob = PathJoin( pathCStr, "*" );

  WIN32_FIND_DATAA fd;
  ::ZeroMemory( &fd, sizeof( fd ) );

  HANDLE hDir = ::FindFirstFileA( searchGlob.c_str(), &fd );
  if( hDir == INVALID_HANDLE_VALUE )
    return false;
  do
  {
    // [pzion 20130723] Skip . and ..
    char const *entryCStr = fd.cFileName;
    if ( entryCStr[0] == '.'
      && (entryCStr[1] == '\0'
        || (entryCStr[1] == '.' && entryCStr[2] == '\0')
        )
      )
      continue;

    entries.push_back( entryCStr );
  } while( ::FindNextFileA( hDir, &fd ) );
  ::FindClose( hDir );
#endif

  return true;
}

inline bool FSDirAppendEntries(
  std::string const &path,
  std::vector<std::string> &entries
  )
{
  return FSDirAppendEntries( path.c_str(), entries );
}

inline bool FSDirAppendEntries(
  StrRef pathStr,
  std::vector<std::string> &entries
  )
{
  char *pathCStr = (char *)alloca( pathStr.size() + 1 );
  memcpy( pathCStr, pathStr.data(), pathStr.size() );
  pathCStr[pathStr.size()] = '\0';
  return FSDirAppendEntries( pathCStr, entries );
}

inline bool FSMaybeDeleteFile(
  FTL::CStrRef filenameCStr
  )
{
#if defined(FTL_PLATFORM_POSIX)
  return ::unlink( filenameCStr.c_str() ) == 0;
#elif defined(FTL_PLATFORM_WINDOWS)
  return ::DeleteFileA( filenameCStr.c_str() );
#endif
}

inline bool FSMaybeMoveFile(
  FTL::CStrRef oldFilenameCStr,
  FTL::CStrRef newFilenameCStr
  )
{
#if defined(FTL_PLATFORM_POSIX)
  return ::rename( oldFilenameCStr.c_str(), newFilenameCStr.c_str() ) == 0;
#elif defined(FTL_PLATFORM_WINDOWS)
  return ::MoveFileExA(
    oldFilenameCStr.c_str(),
    newFilenameCStr.c_str(),
    MOVEFILE_REPLACE_EXISTING
    );
#endif
}

FTL_NAMESPACE_END
