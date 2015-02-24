/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MapCharToUpper_h
#define _FTL_MapCharToUpper_h

namespace FTL {

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

} // namespace FTL

#endif //_FTL_MapCharToUpper_h
