/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

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
