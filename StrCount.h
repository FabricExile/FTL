/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrCount_h
#define _FTL_StrCount_h

#include <FTL/MatchCharSingle.h>

#include <string>

namespace FTL {

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

} // namespace FTL

#endif //_FTL_StrCount_h
