/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MapCharSingle_h
#define _FTL_MapCharSingle_h

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

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

FTL_NAMESPACE_END

#endif //_FTL_MapCharSingle_h
