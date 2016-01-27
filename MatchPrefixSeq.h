/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchPrefixSeq_h
#define _FTL_MatchPrefixSeq_h

#include <FTL/Config.h>
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
struct MatchPrefixSeq
{
  MatchPrefixSeq() {}
  bool operator()( StrRef::IT &it, StrRef::IT itEnd  ) const
  {
    StrRef::IT itOrig = it;
    if ( !_0( it, itEnd ) )
      return false;
    if ( !_1( it, itEnd ) )
    {
      it = itOrig;
      return false;
    }
    if ( !_2( it, itEnd ) )
    {
      it = itOrig;
      return false;
    }
    if ( !_3( it, itEnd ) )
    {
      it = itOrig;
      return false;
    }
    if ( !_4( it, itEnd ) )
    {
      it = itOrig;
      return false;
    }
    if ( !_5( it, itEnd ) )
    {
      it = itOrig;
      return false;
    }
    if ( !_6( it, itEnd ) )
    {
      it = itOrig;
      return false;
    }
    if ( !_7( it, itEnd ) )
    {
      it = itOrig;
      return false;
    }
    if ( !_8( it, itEnd ) )
    {
      it = itOrig;
      return false;
    }
    if ( !_9( it, itEnd ) )
    {
      it = itOrig;
      return false;
    }
    return true;
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

#endif //_FTL_MatchPrefixSeq_h
