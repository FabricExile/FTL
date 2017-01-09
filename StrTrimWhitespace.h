/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/MatchCharWhitespace.h>
#include <FTL/StrTrim.h>

FTL_NAMESPACE_BEGIN

inline void StrTrimWhitespace( std::string &str )
{
  StrTrim<MatchCharWhitespace>( str );
}

FTL_NAMESPACE_END
