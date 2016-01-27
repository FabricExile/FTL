/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrTrim_h
#define _FTL_StrTrim_h

#include <FTL/Config.h>
#include <FTL/MatchCharSingle.h>

#include <string>

FTL_NAMESPACE_BEGIN

template<typename MatchChar>
void StrTrimLeft( std::string &str )
{
  MatchChar const mc;

  std::string::const_iterator const itBegin = str.begin();
  std::string::const_iterator const itEnd = str.end();

  std::string::const_iterator itLeft = itBegin;
  for (;;)
  {
    if ( itLeft == itEnd )
      break;
    if ( !mc( *itLeft ) )
      break;
    ++itLeft;
  }

  if ( itLeft != itBegin )
  {
    memmove( &str[0], &str[itLeft - itBegin], itEnd - itLeft );
    str.resize( itEnd - itLeft );
  }
}

template<char CharToMatch>
void StrTrimLeft( std::string &str )
{
  StrTrimLeft< MatchCharSingle<CharToMatch> >( str );
}

template<typename MatchChar>
void StrTrimRight( std::string &str )
{
  MatchChar const mc;

  std::string::const_iterator const itBegin = str.begin();
  std::string::const_iterator const itEnd = str.end();

  std::string::const_iterator itRight = itEnd;
  for (;;)
  {
    if ( itRight == itBegin )
      break;
    if ( !mc( *(itRight-1) ) )
      break;
    --itRight;
  }

  if ( itRight != itEnd )
    str.resize( itRight - itBegin );
}

template<char CharToMatch>
void StrTrimRight( std::string &str )
{
  StrTrimRight< MatchCharSingle<CharToMatch> >( str );
}

template<typename MatchChar>
void StrTrim( std::string &str )
{
  StrTrimLeft<MatchChar>( str );
  StrTrimRight<MatchChar>( str );
}

template<char CharToMatch>
void StrTrim( std::string &str )
{
  StrTrim< MatchCharSingle<CharToMatch> >( str );
}

FTL_NAMESPACE_END

#endif //_FTL_StrTrim_h
