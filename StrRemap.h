/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrRemap_h
#define _FTL_StrRemap_h

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

#endif //_FTL_StrRemap_h
