/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchPrefixOneOrMore_h
#define _FTL_MatchPrefixOneOrMore_h

#include <FTL/StrRef.h>

namespace FTL {

template<typename MatchPrefix>
struct MatchPrefixOneOrMore
{
  MatchPrefixOneOrMore() {}
  bool operator()( StrRef::IT &it, StrRef::IT itEnd  ) const
  {
    if ( !_mp( it, itEnd ) )
      return false;
    while ( _mp( it, itEnd ) ) ;
    return true;
  }
private:
  MatchPrefix _mp;
};

} // namespace FTL

#endif //_FTL_MatchPrefixOneOrMore_h
