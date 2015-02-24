/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrSplit_h
#define _FTL_StrSplit_h

#include <FTL/MatchCharSingle.h>

#include <string>

namespace FTL {

template<typename FnMatch>
void StrSplit(
  char const *cStr,
  std::vector<std::string> &list,
  bool strict = false
  )
{
  FnMatch const mf;

  for (;;)
  {
    char const *p = cStr;
    while ( *p && !mf( *p ) )
      ++p;
    if ( strict || p != cStr )
      list.push_back( std::string( cStr, p ) );
    if ( !*p )
      break;
    else
      cStr = p + 1;
  }
}

template<typename FnMatch>
void StrSplit(
  std::string const &str,
  std::vector<std::string> &list,
  bool strict = false
  )
{
  return StrSplit<FnMatch>( str.c_str(), list, strict );
}

template<char CharToMatch>
void StrSplit(
  char const *cStr,
  std::vector<std::string> &list,
  bool strict = false
  )
{
  StrSplit< MatchCharSingle<CharToMatch> >( cStr, list, strict );
}

template<char CharToMatch>
void StrSplit(
  std::string const &str,
  std::vector<std::string> &list,
  bool strict = false
  )
{
  StrSplit< MatchCharSingle<CharToMatch> >( str, list, strict );
}

} // namespace FTL

#endif //_FTL_StrSplit_h
