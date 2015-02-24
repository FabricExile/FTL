/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchPrefixNever_h
#define _FTL_MatchPrefixNever_h

#include <FTL/MatchPrefixRange.h>

namespace FTL {

struct MatchPrefixNever
{
  MatchPrefixNever() {}
  bool operator()( MatchPrefixRange &r  ) const
  {
    return false;
  }
};

} // namespace FTL

#endif //_FTL_MatchPrefixNever_h
