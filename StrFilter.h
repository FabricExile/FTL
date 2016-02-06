/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

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
