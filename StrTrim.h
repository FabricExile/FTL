/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrTrim_h
#define _FTL_StrTrim_h

#include <string>

namespace FTL {

template<typename FnMatch>
void StrTrimLeft( std::string &str )
{
  FnMatch const mf;

  std::string::const_iterator const itBegin = str.begin();
  std::string::const_iterator const itEnd = str.end();

  std::string::const_iterator itLeft = itBegin;
  for (;;)
  {
    if ( itLeft == itEnd )
      break;
    if ( !mf( *itLeft ) )
      break;
    ++itLeft;
  }

  if ( itLeft != itBegin )
  {
    memmove( &str[0], &str[itLeft - itBegin], itEnd - itLeft );
    str.resize( itEnd - itLeft );
  }
}

template<typename FnMatch>
void StrTrimRight( std::string &str )
{
  FnMatch const mf;

  std::string::const_iterator const itBegin = str.begin();
  std::string::const_iterator const itEnd = str.end();

  std::string::const_iterator itRight = itEnd;
  for (;;)
  {
    if ( itRight == itBegin )
      break;
    if ( !mf( *(itRight-1) ) )
      break;
    --itRight;
  }

  if ( itRight != itEnd )
    str.resize( itRight - itBegin );
}

template<typename FnMatch>
void StrTrim( std::string &str )
{
  StrTrimLeft<FnMatch>( str );
  StrTrimRight<FnMatch>( str );
}

} // namespace FTL

#endif //_FTL_StrTrim_h
