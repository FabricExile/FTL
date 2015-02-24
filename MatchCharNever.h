/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharNever_h
#define _FTL_MatchCharNever_h

namespace FTL {

struct MatchCharNever
{
  MatchCharNever() {}
  bool operator()( char ch ) const
  {
    return false;
  }
};

} // namespace FTL

#endif //_FTL_MatchCharNever_h
