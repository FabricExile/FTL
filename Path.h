/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_Path_h
#define _FTL_Path_h

#include <FTL/Config.h>
#include <FTL/MatchChar.h>
#include <FTL/MatchPrefix.h>

#include <assert.h>
#include <string>

FTL_NAMESPACE_BEGIN

#if defined(FTL_PLATFORM_POSIX)
static const char PathSep_Platform = '/';
static const char PathSep_NonPlatform = '\\';
#elif defined(FTL_PLATFORM_WINDOWS)
static const char PathSep_Platform = '\\';
static const char PathSep_NonPlatform = '/';
#else
# error "Unsupported platform"
#endif

static const char PathSep_Override = '/';

inline char PathSep( bool overridePlatform = false )
{
  return overridePlatform? PathSep_Override: PathSep_Platform;
}

template<typename StringTy>
inline void PathAppendEntry(
  StringTy &path,
  StrRef entry,
  bool overridePlatform = false
  )
{
  char pathSep = PathSep( overridePlatform );
  if ( (!entry.empty() && entry.front() != pathSep)
    && (!path.empty() && path[path.size()-1] != pathSep) )
    path += pathSep;
  path += entry;
}

inline std::string PathJoin(
  StrRef pathStr0,
  StrRef pathStr1,
  bool overridePlatform = false
  )
{
  std::string result;
  PathAppendEntry( result, pathStr0, overridePlatform );
  PathAppendEntry( result, pathStr1, overridePlatform );
  return result;
}

inline std::string PathJoin(
  StrRef pathStr0,
  StrRef pathStr1,
  StrRef pathStr2,
  bool overridePlatform = false
  )
{
  std::string result;
  PathAppendEntry( result, pathStr0, overridePlatform );
  PathAppendEntry( result, pathStr1, overridePlatform );
  PathAppendEntry( result, pathStr2, overridePlatform );
  return result;
}

inline std::string PathJoin(
  StrRef pathStr0,
  StrRef pathStr1,
  StrRef pathStr2,
  StrRef pathStr3,
  bool overridePlatform = false
  )
{
  std::string result;
  PathAppendEntry( result, pathStr0, overridePlatform );
  PathAppendEntry( result, pathStr1, overridePlatform );
  PathAppendEntry( result, pathStr2, overridePlatform );
  PathAppendEntry( result, pathStr3, overridePlatform );
  return result;
}

inline StrRef::Split PathSplit( StrRef path )
{
  std::pair<StrRef, StrRef> split = path.rsplit( PathSep_Platform );
  if ( !split.second.empty() )
    return split;
  return path.rsplit( PathSep_NonPlatform );
}

typedef
#if defined(FTL_PLATFORM_POSIX)
  MatchPrefixChar< MatchCharSingle<PathSep_Platform> >
#elif defined(FTL_PLATFORM_WINDOWS)
  MatchPrefixAny<
    MatchPrefixChar< MatchCharSingle<PathSep_Platform> >,
    MatchPrefixSeq<
      MatchPrefixChar<
        MatchCharAny<
          MatchCharRange<'A', 'Z'>,
          MatchCharRange<'a', 'z'>
          >
        >,
      MatchPrefixChar< MatchCharSingle<':'> >
      >
    >
#endif
  MatchPrefixAbsolutePath;

inline bool PathIsAbsolute( StrRef path )
{
  StrRef::IT it = path.begin();
  return MatchPrefixAbsolutePath()( it, path.end() );
}

FTL_NAMESPACE_END

#endif //_FTL_Path_h
