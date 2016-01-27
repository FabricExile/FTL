/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MapCharToLower_h
#define _FTL_MapCharToLower_h

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

struct MapCharToLower
{
  MapCharToLower() {}
  char operator()( char ch ) const
  {
    if ( ch >= 'A' && ch <= 'Z' )
    {
      ch -= 'A';
      ch += 'a';
    }
    return ch;
  }
};

FTL_NAMESPACE_END

#endif //_FTL_MapCharToLower_h
