/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MapCharSingle_h
#define _FTL_MapCharSingle_h

namespace FTL {

template<char SrcChar, char DstChar>
struct MapCharSingle
{
  MapCharSingle() {}
  char operator()( char ch ) const
  {
    if ( ch == SrcChar )
      ch = DstChar;
    return ch;
  }
};

} // namespace FTL

#endif //_FTL_MapCharSingle_h
