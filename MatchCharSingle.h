/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharSingle_h
#define _FTL_MatchCharSingle_h

namespace FTL {

template<char CharToMatch>
struct MatchCharSingle
{
  MatchCharSingle() {}
  bool operator()( char ch ) const
  {
    return ch == CharToMatch;
  }
};

} // namespace FTL

#endif //_FTL_MatchCharSingle_h
