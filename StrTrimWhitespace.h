/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrTrimWhitespace_h
#define _FTL_StrTrimWhitespace_h

#include <FTL/Config.h>
#include <FTL/MatchCharWhitespace.h>
#include <FTL/StrTrim.h>

FTL_NAMESPACE_BEGIN

inline void StrTrimWhitespace( std::string &str )
{
  StrTrim<MatchCharWhitespace>( str );
}

FTL_NAMESPACE_END

#endif //_FTL_StrTrimWhitespace_h
