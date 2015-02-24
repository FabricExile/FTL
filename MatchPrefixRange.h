/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchPrefixRange_h
#define _FTL_MatchPrefixRange_h

#include <FTL/MatchPrefixPos.h>

namespace FTL {

struct MatchPrefixRange
{
  MatchPrefixPos b, e;

  MatchPrefixRange( MatchPrefixPos _b, MatchPrefixPos _e ) :
    b( _b ), e ( _e ) {}

  bool empty() const { return b == e; }

  static MatchPrefixRange Empty( MatchPrefixPos p )
  {
    return MatchPrefixRange( p, p );
  }
};

} // namespace FTL

#endif //_FTL_MatchPrefixRange_h
