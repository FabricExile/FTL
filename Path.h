/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
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
static const char PathSep = '/';
#elif defined(FTL_PLATFORM_WINDOWS)
static const char PathSep = '\\';
#else
# error "Unsupported platform"
#endif

template<typename StringTy>
inline void PathAppendEntry(
  StringTy &path,
  StrRef entry
  )
{
  if ( (!entry.empty() && entry.front() != PathSep)
    && (!path.empty() && path[path.size()-1] != PathSep) )
    path += PathSep;
  path += entry;
}

inline std::string PathJoin(
  StrRef lhsPathStr,
  StrRef rhsPathStr
  )
{
  std::string result;
  PathAppendEntry( result, lhsPathStr );
  PathAppendEntry( result, rhsPathStr );
  return result;
}

inline std::string PathNorm( StrRef path )
{
  std::string result(path.begin(), path.end());
  for(unsigned int i=0;i<result.length(); i++)
  {
    if(result[i] == '/' || result[i] == '\\')
      result[i] = PathSep;
  }
  return result;
}

inline std::pair<StrRef, StrRef> PathSplit( StrRef path )
{
  return path.rsplit( PathSep );
}

typedef
#if defined(FTL_PLATFORM_POSIX)
  MatchPrefixChar< MatchCharSingle<PathSep> >
#elif defined(FTL_PLATFORM_WINDOWS)
  MatchPrefixAny<
    MatchPrefixChar< MatchCharSingle<PathSep> >,
    MatchPrefixSeq<
      MatchPrefixChar<
        MatchCharAny<
          MatchCharRange<'A', 'Z'>,
          MatchCharRange<'a', 'z'>
          >
        >,
      MatchPrefixChar< MatchCharSingle<':'> >,
      MatchPrefixChar< MatchCharSingle<PathSep> >
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
