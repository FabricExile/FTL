/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/MatchCharSingle.h>
#include <FTL/StrSplit.h>

#include <stdlib.h>

FTL_NAMESPACE_BEGIN

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

FTL_NAMESPACE_END
