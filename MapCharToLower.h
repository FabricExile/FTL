/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MapCharToLower_h
#define _FTL_MapCharToLower_h

namespace FTL {

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

} // namespace FTL

#endif //_FTL_MapCharToLower_h
