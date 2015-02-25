/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_Env_h
#define _FTL_Env_h

#include <FTL/Config.h>
#include <FTL/MatchCharSingle.h>
#include <FTL/StrSplit.h>

#include <stdlib.h>

namespace FTL {

#if defined(FTL_PLATFORM_POSIX)
static const char EnvListSep = ':';
#elif defined(FTL_PLATFORM_WINDOWS)
static const char EnvListSep = ';';
#else
# error "Unsupported platform"
#endif

inline bool EnvGetList(
  char const *k,
  std::vector<std::string> &list,
  bool strict = false
  )
{
  char const *v = ::getenv( k );
  if ( !v )
    return false;

  StrSplit<EnvListSep>( v, list, strict );

  return true;
}

} // namespace FTL

#endif //_FTL_Env_h
