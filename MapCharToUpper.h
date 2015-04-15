/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MapCharToUpper_h
#define _FTL_MapCharToUpper_h

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

struct MapCharToUpper
{
  MapCharToUpper() {}
  char operator()( char ch ) const
  {
    if ( ch >= 'a' && ch <= 'z' )
    {
      ch -= 'a';
      ch += 'A';
    }
    return ch;
  }
};

FTL_NAMESPACE_END

#endif //_FTL_MapCharToUpper_h
