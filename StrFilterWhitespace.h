/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrFilterWhitespace_h
#define _FTL_StrFilterWhitespace_h

#include <FTL/Config.h>
#include <FTL/MatchCharWhitespace.h>
#include <FTL/StrFilter.h>

FTL_NAMESPACE_BEGIN

inline std::string StrFilterWhitespace( char const *cStr )
{
  return StrFilter<MatchCharWhitespace>( cStr );
}

inline std::string StrFilterWhitespace( std::string const &str )
{
  return StrFilter<MatchCharWhitespace>( str );
}

FTL_NAMESPACE_END

#endif //_FTL_StrFilterWhitespace_h
