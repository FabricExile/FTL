/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrToLower_h
#define _FTL_StrToLower_h

#include <FTL/MapCharToLower.h>
#include <FTL/StrRemap.h>

namespace FTL {

inline void StrToLower( std::string &str )
{
  StrRemap<MapCharToLower>( str );
}

} // namespace FTL

#endif //_FTL_StrToLower_h
