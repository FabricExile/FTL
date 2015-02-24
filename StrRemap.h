/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrRemap_h
#define _FTL_StrRemap_h

#include <string>

namespace FTL {

template<typename FnMap>
void StrRemap( std::string &str )
{
  FnMap const mf;

  std::string::iterator const itBegin = str.begin();
  std::string::iterator const itEnd = str.end();
  for ( std::string::iterator it = itBegin; it != itEnd; ++it )
    *it = mf( *it );
}

} // namespace FTL

#endif //_FTL_StrRemap_h
