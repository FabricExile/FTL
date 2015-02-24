/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchPrefixOneOrMore_h
#define _FTL_MatchPrefixOneOrMore_h

#include <FTL/MatchPrefixRange.h>

namespace FTL {

template<typename MatchPrefix>
struct MatchPrefixOneOrMore
{
  MatchPrefixOneOrMore() {}
  bool operator()( MatchPrefixRange &r  ) const
  {
    if ( !_mp( r ) )
      return false;
    while ( _mp( r ) ) ;
    return true;
  }
private:
  MatchPrefix _mp;
};

} // namespace FTL

#endif //_FTL_MatchPrefixOneOrMore_h
