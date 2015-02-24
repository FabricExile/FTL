/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrFilterWhitespace_h
#define _FTL_StrFilterWhitespace_h

#include <FTL/MatchCharWhitespace.h>
#include <FTL/StrFilter.h>

namespace FTL {

inline std::string StrFilterWhitespace( char const *cStr )
{
  return StrFilter<MatchCharWhitespace>( cStr );
}

inline std::string StrFilterWhitespace( std::string const &str )
{
  return StrFilter<MatchCharWhitespace>( str );
}

} // namespace FTL

#endif //_FTL_StrFilterWhitespace_h
