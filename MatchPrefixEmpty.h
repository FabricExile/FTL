/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/StrRef.h>

FTL_NAMESPACE_BEGIN

struct MatchPrefixEmpty
{
  MatchPrefixEmpty() {}
  bool operator()( StrRef::IT &it, StrRef::IT itEnd  ) const
  {
    return true;
  }
};

FTL_NAMESPACE_END
