/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/MapCharToLower.h>
#include <FTL/StrRemap.h>

FTL_NAMESPACE_BEGIN

inline void StrToLower( std::string &str )
{
  StrRemap<MapCharToLower>( str );
}

FTL_NAMESPACE_END
