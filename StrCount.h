/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/MatchCharSingle.h>

#include <string>

FTL_NAMESPACE_BEGIN

template<typename MatchChar>
size_t StrCount( char const *cStr )
{
  MatchChar const fn;

  size_t count = 0;
  while ( *cStr )
  {
    if ( fn( *cStr++ ) )
      ++count;
  }
  return count;
}

template<typename MatchChar>
size_t StrCount( std::string const &str )
{
  return StrCount<MatchChar>( str.c_str() );
}

template<char CharToMatch>
size_t StrCount( char const *cStr )
{
  return StrCount< MatchCharSingle<CharToMatch> >( cStr );
}

template<char CharToMatch>
size_t StrCount( std::string const &str )
{
  return StrCount< MatchCharSingle<CharToMatch> >( str );
}

FTL_NAMESPACE_END
