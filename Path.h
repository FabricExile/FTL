/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_Path_h
#define _FTL_Path_h

#include <FTL/Config.h>

#include <assert.h>
#include <string>

namespace FTL {

#if defined(FABRIC_PLATFORM_POSIX)
static const char PathSep = '/';
#elif defined(FABRIC_PLATFORM_WINDOWS)
static const char PathSep = '\\';
#else
# error "Unsupported platform"
#endif

inline void PathAppendEntry(
  std::string &path,
  char const *entryCStr
  )
{
  bool entryNonEmpty = entryCStr && *entryCStr;
  size_t pathSize = path.size();
  if ( pathSize > 0
    && path[pathSize-1] != PathSep
    && entryNonEmpty )
  {
    path += PathSep;
    if ( entryNonEmpty )
    {
      assert( *entryCStr != PathSep );
      path += entryCStr;
    }
  }
}

inline void PathAppendEntry(
  std::string &path,
  std::string const &entry
  )
{
  PathAppendEntry( path, entry.c_str() );
}

inline std::string PathJoin(
  char const *lhsPathCStr,
  char const *rhsPathCStr
  )
{
  std::string result;
  PathAppendEntry( result, lhsPathCStr );
  PathAppendEntry( result, rhsPathCStr );
  return result;
}

inline std::string PathJoin(
  std::string const &lhsPathStr,
  char const *rhsPathCStr
  )
{
  std::string result;
  PathAppendEntry( result, lhsPathStr );
  PathAppendEntry( result, rhsPathCStr );
  return result;
}

inline std::string PathJoin(
  char const *lhsPathCStr,
  std::string const &rhsPathStr
  )
{
  std::string result;
  PathAppendEntry( result, lhsPathCStr );
  PathAppendEntry( result, rhsPathStr );
  return result;
}

inline std::string PathJoin(
  std::string const &lhsPathStr,
  std::string const &rhsPathStr
  )
{
  std::string result;
  PathAppendEntry( result, lhsPathStr );
  PathAppendEntry( result, rhsPathStr );
  return result;
}

} // namespace FTL

#endif //_FTL_Path_h
