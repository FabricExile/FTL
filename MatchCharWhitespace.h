/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchCharWhitespace_h
#define _FTL_MatchCharWhitespace_h

#include <FTL/Config.h>

#include <ctype.h>

namespace FTL {

struct MatchCharWhitespace
{
  MatchCharWhitespace() {}
  bool operator()( char ch ) const
  {
    return isspace( ch );
  }
};

} // namespace FTL

#endif //_FTL_MatchCharWhitespace_h
