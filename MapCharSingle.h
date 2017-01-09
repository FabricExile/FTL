/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

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
