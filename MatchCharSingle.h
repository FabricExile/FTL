/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharSingle_h
#define _FTL_MatchCharSingle_h

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

template<char CharToMatch>
struct MatchCharSingle
{
  MatchCharSingle() {}
  bool operator()( char ch ) const
  {
    return ch == CharToMatch;
  }
};

FTL_NAMESPACE_END

#endif //_FTL_MatchCharSingle_h
