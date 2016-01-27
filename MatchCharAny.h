/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharAny_h
#define _FTL_MatchCharAny_h

#include <FTL/Config.h>
#include <FTL/MatchCharNever.h>

FTL_NAMESPACE_BEGIN

template<
  typename MatchChar0,
  typename MatchChar1 = MatchCharNever,
  typename MatchChar2 = MatchCharNever,
  typename MatchChar3 = MatchCharNever,
  typename MatchChar4 = MatchCharNever,
  typename MatchChar5 = MatchCharNever,
  typename MatchChar6 = MatchCharNever,
  typename MatchChar7 = MatchCharNever,
  typename MatchChar8 = MatchCharNever,
  typename MatchChar9 = MatchCharNever
  >
struct MatchCharAny
{
  MatchCharAny() {}
  bool operator()( char ch ) const
  {
    return _0(ch) || _1(ch) || _2(ch) || _3(ch) || _4(ch)
      || _5(ch) || _6(ch) || _7(ch) || _8(ch) || _9(ch);
  }
private:
  MatchChar0 _0;
  MatchChar1 _1;
  MatchChar2 _2;
  MatchChar3 _3;
  MatchChar4 _4;
  MatchChar5 _5;
  MatchChar6 _6;
  MatchChar7 _7;
  MatchChar8 _8;
  MatchChar9 _9;
};

FTL_NAMESPACE_END

#endif //_FTL_MatchCharAny_h
