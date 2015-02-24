/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_MatchPrefixChar_h
#define _FTL_MatchPrefixChar_h

#include <FTL/MatchPrefixRange.h>

namespace FTL {

template<typename MatchChar>
struct MatchPrefixChar
{
  MatchPrefixChar() {}
  bool operator()( MatchPrefixRange &r  ) const
  {
    if ( !r.empty() && _matchChar( *r.b ) )
    {
      ++r.b;
      return true;
    }
    else return false;
  }
private:
  MatchChar _matchChar;
};

} // namespace FTL

#endif //_FTL_MatchPrefixChar_h
