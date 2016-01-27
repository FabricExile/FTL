/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharWhitespace_h
#define _FTL_MatchCharWhitespace_h

#include <FTL/Config.h>

#include <ctype.h>

FTL_NAMESPACE_BEGIN

struct MatchCharWhitespace
{
  MatchCharWhitespace() {}
  bool operator()( char ch ) const
  {
    return isspace( ch );
  }
};

FTL_NAMESPACE_END

#endif //_FTL_MatchCharWhitespace_h
