/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrFilter_h
#define _FTL_StrFilter_h

#include <FTL/Config.h>
#include <string>

FTL_NAMESPACE_BEGIN

template<typename MatchChar>
std::string StrFilter( char const *cStr )
{
  MatchChar const fn;

  std::string result;
  for (;;)
  {
    char ch = *cStr++;
    if ( !ch )
      break;
    else if ( !fn( ch ) )
      result += ch;
  }
  return result;
}

template<typename MatchChar>
std::string StrFilter( std::string const &str )
{
  return StrFilter<MatchChar>( str.c_str() );
}

FTL_NAMESPACE_END

#endif //_FTL_StrFilter_h
