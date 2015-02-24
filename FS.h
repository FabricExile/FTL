/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_FS_h
#define _FTL_FS_h

#include <FTL/Config.h>
#include <FTL/Path.h>

#include <string>
#include <vector>
#if defined(FABRIC_PLATFORM_POSIX)
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <pwd.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>
#elif defined(FABRIC_PLATFORM_WINDOWS)
# include <Windows.h>
#else
# error "Unsupport FABRIC_PLATFORM_..."
#endif

namespace FTL {

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

#if defined(FABRIC_PLATFORM_POSIX)
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
#elif defined(FABRIC_PLATFORM_WINDOWS)
  DWORD fa = ::GetFileAttributesA( pathCStr );
  if ( fa == INVALID_FILE_ATTRIBUTES )
    return false;

  if ( fa & FILE_ATTRIBUTE_DIRECTORY )
    statInfo.type = FSStatInfo::Dir;
  else if ( fa & FILE_ATTRIBUTE_NORMAL )
    statInfo.type = FSStatInfo::File;
  else
    statInfo.type = FSStatInfo::Other;

  return true;
#endif
}

inline bool FSStat(
  std::string const &path,
  FSStatInfo &statInfo
  )
{
  return FSStat( path.c_str(), statInfo );
}

inline bool FSExists( char const *pathCStr )
{
  FSStatInfo statInfo;
  return FSStat( pathCStr, statInfo );
}

inline bool FSExists( std::string const &path )
{
  return FSExists( path.c_str() );
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

inline bool FSMkDir( char const *pathCStr )
{
  FSStatInfo statInfo;
  if ( FSStat( pathCStr, statInfo ) )
    return statInfo.type == FSStatInfo::Dir;

  if ( !pathCStr || !*pathCStr )
    return true;
  
#if defined(FABRIC_PLATFORM_POSIX)
  return ::mkdir( pathCStr, 0777 ) == 0;
#elif defined(FABRIC_PLATFORM_WINDOWS)
  return ::CreateDirectory( pathCStr, NULL );
#endif
}

inline bool FSMkDir( std::string const &path )
{
  return FSMkDir( path.c_str() );
}

inline bool FSMkDirRecursive( char const *pathCStr )
{
  if ( !pathCStr )
    return true;

  char const *lastPathSepCStr = 0;
  for ( char const *p = pathCStr; *p; ++p )
    if ( *p == PathSep )
      lastPathSepCStr = p;

  if ( lastPathSepCStr )
  {
    std::string parentDir( pathCStr, lastPathSepCStr );
    if ( !FSMkDirRecursive( parentDir.c_str() ) )
      return false;
  }
  return FSMkDir( pathCStr );
}

inline bool FSMkDirRecursive( std::string const &dir )
{
  return FSMkDirRecursive( dir.c_str() );
}

inline bool FSDirAppendEntries(
  char const *pathCStr,
  std::vector<std::string> &entries
  )
{
  if ( !pathCStr || !*pathCStr )
    pathCStr = ".";

#if defined(FABRIC_PLATFORM_POSIX)
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
#elif defined(FABRIC_OS_WINDOWS)
  std::string searchGlob = FSPathJoin( pathCStr, "*" );

  WIN32_FIND_DATAA fd;
  ::ZeroMemory( &fd, sizeof( fd ) );

  HANDLE hDir = ::FindFirstFileA( searchGlob.c_str(), &fd );
  if( hDir == INVALID_HANDLE_VALUE )
    return false;
  do
  {
    if( !( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
      continue;

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

} // namespace FTL

#endif //_FTL_FS_h
