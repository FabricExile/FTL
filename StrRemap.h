/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>

#include <string>

FTL_NAMESPACE_BEGIN

template<typename FnMap>
void StrRemap( std::string &str )
{
  FnMap const mf;

  std::string::iterator const itBegin = str.begin();
  std::string::iterator const itEnd = str.end();
  for ( std::string::iterator it = itBegin; it != itEnd; ++it )
    *it = mf( *it );
}

FTL_NAMESPACE_END
