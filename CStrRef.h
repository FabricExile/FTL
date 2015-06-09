/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_CStrRef_h
#define _FTL_CStrRef_h

#include <FTL/StrRef.h>

FTL_NAMESPACE_BEGIN

class CStrRef : public StrRef
{
public:

  CStrRef() : StrRef() {}
  CStrRef( char const *cStr ) : StrRef( cStr ) {}
  CStrRef( char const *cStr, size_t length ) : StrRef( cStr, length )
    { assert( !cStr || cStr[length] == '\0' ); }
  CStrRef( std::string const &str ) : StrRef( str.c_str(), str.size() ) {}

  char const *c_str() const { return data(); }

  std::pair<StrRef, CStrRef> split( char ch ) const
  {
    IT it = find( ch );
    if ( it == end() )
      return std::pair<StrRef, CStrRef>( *this, CStrRef() );
    else
      return std::pair<StrRef, CStrRef>(
        StrRef( begin(), it ),
        CStrRef( it + 1, end() )
        );
  }

  std::pair<StrRef, CStrRef> rsplit( char ch ) const
  {
    RIT it = rfind( ch );
    if ( it == rend() )
      return std::pair<StrRef, CStrRef>( StrRef(), *this );
    else
      return std::pair<StrRef, CStrRef>(
        StrRef( it + 1, rend() ),
        CStrRef( rbegin(), it )
        );
  }

private:

  CStrRef( IT b, IT e ) : StrRef( b, e ) { assert( *e == '\0' ); }
  CStrRef( RIT b, RIT e ) : StrRef( b, e ) { assert( *b.base() == '\0' ); }
};

FTL_NAMESPACE_END

#endif //_FTL_CStrRef_h
