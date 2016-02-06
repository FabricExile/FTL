/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>

#include <ctype.h>

FTL_NAMESPACE_BEGIN

struct MatchCharWhitespace
{
  MatchCharWhitespace() {}
  bool operator()( char ch ) const
  {
    return isspace( ch );
  }
};

FTL_NAMESPACE_END
