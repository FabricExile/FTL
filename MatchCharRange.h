/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharRange_h
#define _FTL_MatchCharRange_h

namespace FTL {

template<char BeginCharToMatch, char EndCharToMatch>
struct MatchCharRange
{
  MatchCharRange() {}
  bool operator()( char ch ) const
  {
    return ch >= BeginCharToMatch && ch <= EndCharToMatch;
  }
};

} // namespace FTL

#endif //_FTL_MatchCharRange_h
