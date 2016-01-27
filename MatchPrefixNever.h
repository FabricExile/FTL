/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchPrefixNever_h
#define _FTL_MatchPrefixNever_h

#include <FTL/Config.h>
#include <FTL/StrRef.h>

FTL_NAMESPACE_BEGIN

struct MatchPrefixNever
{
  MatchPrefixNever() {}
  bool operator()( StrRef::IT &it, StrRef::IT itEnd  ) const
  {
    return false;
  }
};

FTL_NAMESPACE_END

#endif //_FTL_MatchPrefixNever_h
