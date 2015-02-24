/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchPrefixSeq_h
#define _FTL_MatchPrefixSeq_h

#include <FTL/MatchPrefixRange.h>

namespace FTL {

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
struct MatchPrefixSeq
{
  MatchPrefixSeq() {}
  bool operator()( MatchPrefixRange &r  ) const
  {
    if ( !_0( r ) )
      return false;
    if ( !_1( r ) )
      return false;
    if ( !_2( r ) )
      return false;
    if ( !_3( r ) )
      return false;
    if ( !_4( r ) )
      return false;
    if ( !_5( r ) )
      return false;
    if ( !_6( r ) )
      return false;
    if ( !_7( r ) )
      return false;
    if ( !_8( r ) )
      return false;
    if ( !_9( r ) )
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

} // namespace FTL

#endif //_FTL_MatchPrefixSeq_h
