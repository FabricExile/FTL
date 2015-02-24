/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharAlways_h
#define _FTL_MatchCharAlways_h

namespace FTL {

struct MatchCharAlways
{
  MatchCharAlways() {}
  bool operator()( char ch ) const
  {
    return true;
  }
};

} // namespace FTL

#endif //_FTL_MatchCharAlways_h
