/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharAlways_h
#define _FTL_MatchCharAlways_h

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

struct MatchCharAlways
{
  MatchCharAlways() {}
  bool operator()( char ch ) const
  {
    return true;
  }
};

FTL_NAMESPACE_END

#endif //_FTL_MatchCharAlways_h
