/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharNever_h
#define _FTL_MatchCharNever_h

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

struct MatchCharNever
{
  MatchCharNever() {}
  bool operator()( char ch ) const
  {
    return false;
  }
};

FTL_NAMESPACE_END

#endif //_FTL_MatchCharNever_h
