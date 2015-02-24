/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrTrimWhitespace_h
#define _FTL_StrTrimWhitespace_h

#include <FTL/MatchCharWhitespace.h>
#include <FTL/StrTrim.h>

namespace FTL {

inline void StrTrimWhitespace( std::string &str )
{
  StrTrim<MatchCharWhitespace>( str );
}

} // namespace FTL

#endif //_FTL_StrTrimWhitespace_h
