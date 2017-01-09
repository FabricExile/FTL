/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

struct MatchCharNever
{
  MatchCharNever() {}
  bool operator()( char ch ) const
  {
    return false;
  }
};

FTL_NAMESPACE_END
