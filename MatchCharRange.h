/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

template<char BeginCharToMatch, char EndCharToMatch>
struct MatchCharRange
{
  MatchCharRange() {}
  bool operator()( char ch ) const
  {
    return ch >= BeginCharToMatch && ch <= EndCharToMatch;
  }
};

FTL_NAMESPACE_END
