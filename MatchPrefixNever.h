/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/StrRef.h>

FTL_NAMESPACE_BEGIN

struct MatchPrefixNever
{
  MatchPrefixNever() {}
  bool operator()( StrRef::IT &it, StrRef::IT itEnd  ) const
  {
    return false;
  }
};

FTL_NAMESPACE_END
