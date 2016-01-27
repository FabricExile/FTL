/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchPrefixOneOrMore_h
#define _FTL_MatchPrefixOneOrMore_h

#include <FTL/Config.h>
#include <FTL/StrRef.h>

FTL_NAMESPACE_BEGIN

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

FTL_NAMESPACE_END

#endif //_FTL_MatchPrefixOneOrMore_h
