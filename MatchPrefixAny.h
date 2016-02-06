/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/MatchPrefixNever.h>
#include <FTL/StrRef.h>

FTL_NAMESPACE_BEGIN

template<
  typename MatchPrefix0,
  typename MatchPrefix1 = MatchPrefixNever,
  typename MatchPrefix2 = MatchPrefixNever,
  typename MatchPrefix3 = MatchPrefixNever,
  typename MatchPrefix4 = MatchPrefixNever,
  typename MatchPrefix5 = MatchPrefixNever,
  typename MatchPrefix6 = MatchPrefixNever,
  typename MatchPrefix7 = MatchPrefixNever,
  typename MatchPrefix8 = MatchPrefixNever,
  typename MatchPrefix9 = MatchPrefixNever
  >
struct MatchPrefixAny
{
  MatchPrefixAny() {}
  bool operator()( StrRef::IT &it, StrRef::IT itEnd  ) const
  {
    if ( _0( it, itEnd ) )
      return true;
    if ( _1( it, itEnd ) )
      return true;
    if ( _2( it, itEnd ) )
      return true;
    if ( _3( it, itEnd ) )
      return true;
    if ( _4( it, itEnd ) )
      return true;
    if ( _5( it, itEnd ) )
      return true;
    if ( _6( it, itEnd ) )
      return true;
    if ( _7( it, itEnd ) )
      return true;
    if ( _8( it, itEnd ) )
      return true;
    if ( _9( it, itEnd ) )
      return true;
    return false;
  }
private:
  MatchPrefix0 _0;
  MatchPrefix1 _1;
  MatchPrefix2 _2;
  MatchPrefix3 _3;
  MatchPrefix4 _4;
  MatchPrefix5 _5;
  MatchPrefix6 _6;
  MatchPrefix7 _7;
  MatchPrefix8 _8;
  MatchPrefix9 _9;
};

FTL_NAMESPACE_END
