/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

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
