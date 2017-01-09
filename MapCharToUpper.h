/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

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
