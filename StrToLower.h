/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrToLower_h
#define _FTL_StrToLower_h

#include <FTL/Config.h>
#include <FTL/MapCharToLower.h>
#include <FTL/StrRemap.h>

FTL_NAMESPACE_BEGIN

inline void StrToLower( std::string &str )
{
  StrRemap<MapCharToLower>( str );
}

FTL_NAMESPACE_END

#endif //_FTL_StrToLower_h
