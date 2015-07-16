/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_StrRef_h
#define _FTL_StrRef_h

#include <FTL/Config.h>
#include <FTL/MatchCharWhitespace.h>

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <limits.h>
#include <string>
#include <string.h>

FTL_NAMESPACE_BEGIN

class StrRef
{
  char const *_data;
  size_t _size;

public:

  typedef char const *IT;

  IT begin() const { return _data; }
  IT end() const { return _data + _size; }

  typedef std::reverse_iterator<IT> RIT;

  RIT rbegin() const { return RIT( end() ); }
  RIT rend() const { return RIT( begin() ); }

  StrRef() : _data(0), _size(0) {}
  StrRef( char const *cStr )
    : _data( cStr )
    , _size( cStr? strlen( cStr ): 0 )
  {
    assert( _size == 0 || !!_data );
  }
  StrRef( char const *data, size_t size )
    : _data( data )
    , _size( size )
  {
    assert( _size == 0 || !!_data );
  }
  StrRef( std::string const &str ) :
    _data( str.data() ), _size( str.size() ) {}
  StrRef( IT b, IT e )
  {
    assert( b <= e );
    _data = b;
    _size = e - b;
  }
  StrRef( RIT b, RIT e )
  {
    assert( b.base() >= e.base() );
    _data = e.base();
    _size = b.base() - e.base();
  }

  size_t size() const { return _size; }
  bool empty() const { return _size == 0; }
  char const *data() const { return _data; }

  char operator[]( size_t index ) const
  {
    assert( index < _size );
    return _data[index];
  }

  char front() const { return (*this)[0]; }
  StrRef drop_front( size_t count = 1 ) const
    { return StrRef( begin() + count, end() ); }

  char back() const { return (*this)[0]; }
  StrRef drop_back( size_t count = 1 ) const
    { return StrRef( begin(), end() - count ); }

  IT find( IT b, IT e, char ch ) const
  {
    IT it = b;
    while ( it != e )
    {
      if ( *it == ch )
        break;
      ++it;
    }
    return it;
  }

  IT find( char ch ) const
  {
    return find( begin(), end(), ch );
  }

  template<typename MatchChar>
  IT find( IT b, IT e ) const
  {
    MatchChar const mc;
    IT it = b;
    while ( it != e )
    {
      if ( mc( *it ) )
        break;
      ++it;
    }
    return it;
  }

  template<typename MatchChar>
  IT find() const
  {
    return find<MatchChar>( begin(), end() );
  }

  size_t count( IT b, IT e, char ch ) const
  {
    size_t result = 0;
    IT it = b;
    while ( it != e )
    {
      if ( *it == ch )
        ++result;
      ++it;
    }
    return result;
  }

  size_t count( char ch ) const
  {
    return count( begin(), end(), ch );
  }

  template<typename MatchChar>
  size_t count( IT b, IT e ) const
  {
    MatchChar const mc;
    size_t result = 0;
    IT it = b;
    while ( it != e )
    {
      if ( mc( *it ) )
        ++result;
      ++it;
    }
    return result;
  }

  template<typename MatchChar>
  size_t count() const
  {
    return count<MatchChar>( begin(), end() );
  }

  typedef std::pair<StrRef, StrRef> Split;

  Split split( char ch ) const
  {
    IT it = find( ch );
    if ( it == end() )
      return Split( *this, StrRef() );
    else
      return Split(
        StrRef( begin(), it ),
        StrRef( it + 1, end() )
        );
  }

  RIT rfind( RIT rb, RIT re, char ch ) const
  {
    RIT rit = rb;
    while ( rit != re )
    {
      if ( *rit == ch )
        break;
      ++rit;
    }
    return rit;
  }

  RIT rfind( char ch ) const
  {
    return rfind( rbegin(), rend(), ch );
  }

  Split rsplit( char ch ) const
  {
    RIT it = rfind( ch );
    if ( it == rend() )
      return Split( StrRef(), *this );
    else
      return Split(
        StrRef( it + 1, rend() ),
        StrRef( rbegin(), it )
        );
  }

  StrRef substr( unsigned int start, unsigned int length = UINT_MAX ) const
  {
    if( start > size() || length == 0 )
      return "";

    if( start + length >= size() )
      return StrRef( begin() + start, end() );
    else
      return StrRef( begin() + start, begin() + start + length );
  }

  bool contains( char ch )
  {
    return find(ch) != end();
  }

  bool equals( StrRef that ) const
    { return _size == that._size
      && memcmp( _data, that._data, _size ) == 0; }

  bool startswith( StrRef that ) const
    { return _size >= that._size
      && memcmp( _data, that._data, that._size ) == 0; }

  struct Equals
  {
    bool operator ()( StrRef lhs, StrRef rhs ) const
      { return lhs.equals( rhs ); }
  };

  bool operator==( StrRef that ) const
    { return equals( that ); }

  bool operator!=( StrRef that ) const
    { return !equals( that ); }

  int compare( StrRef that ) const
  {
    size_t minSize = (std::min)( _size, that._size );
    int result = memcmp( _data, that._data, minSize );
    if ( result != 0 )
      return result;
    if ( _size < that._size )
      return -1;
    if ( _size > that._size )
      return 1;
    return 0;
  }

  bool operator<( StrRef that ) const
    { return compare( that ) < 0; }

  bool operator<=( StrRef that ) const
    { return compare( that ) <= 0; }

  bool operator>( StrRef that ) const
    { return compare( that ) > 0; }

  bool operator>=( StrRef that ) const
    { return compare( that ) >= 0; }

  size_t hash() const
  {
    // [pzion 20150415] DJB2 algorithm
    size_t result = 5381;
    char const *pEnd = _data + _size;
    for ( char const *p = _data; p != pEnd; ++p )
      result = ((result << 5) + result) + size_t(*p);
    return result;
  }

  struct Hash
  {
    size_t operator ()( StrRef str ) const
      { return str.hash(); }
  };

  template<typename MatchChar>
  StrRef ltrimMatch() const
  {
    MatchChar const mc;
    IT it = begin();
    for ( ; it != end() && mc( *it ); ++it ) ;
    return StrRef( it, end() );
  }

  StrRef ltrim() const
    { return ltrimMatch<MatchCharWhitespace>(); }

  template<typename MatchChar>
  StrRef rtrimMatch() const
  {
    MatchChar const mc;
    RIT it = rbegin();
    for ( ; it != rend() && mc( *it ); ++it ) ;
    return StrRef( it, rend() );
  }

  StrRef rtrim() const
    { return rtrimMatch<MatchCharWhitespace>(); }

  template<typename MatchChar>
  StrRef trimMatch() const
  {
    StrRef partial = ltrimMatch<MatchChar>();
    return partial.rtrimMatch<MatchChar>();
  }

  StrRef trim() const
    { return trimMatch<MatchCharWhitespace>(); }

  operator std::string() const
  {
    return std::string( begin(), end() );
  }
};

class CStrRef : public StrRef
{
public:

  CStrRef() : StrRef() {}
  CStrRef( char const *cStr ) : StrRef( cStr ) {}
  CStrRef( char const *cStr, size_t length ) : StrRef( cStr, length )
    { assert( !cStr || cStr[length] == '\0' ); }
  CStrRef( std::string const &str ) : StrRef( str.c_str(), str.size() ) {}

  char const *c_str() const { return data(); }

  typedef std::pair<StrRef, CStrRef> Split;

  Split split( char ch ) const
  {
    IT it = find( ch );
    if ( it == end() )
      return Split( *this, CStrRef() );
    else
      return Split(
        StrRef( begin(), it ),
        CStrRef( it + 1, end() )
        );
  }

  Split rsplit( char ch ) const
  {
    RIT it = rfind( ch );
    if ( it == rend() )
      return Split( StrRef(), *this );
    else
      return Split(
        StrRef( it + 1, rend() ),
        CStrRef( rbegin(), it )
        );
  }

private:

  CStrRef( IT b, IT e ) : StrRef( b, e ) { assert( *e == '\0' ); }
  CStrRef( RIT b, RIT e ) : StrRef( b, e ) { assert( *b.base() == '\0' ); }
};

FTL_NAMESPACE_END

inline bool operator==( char const *lhs, FTL::StrRef rhs )
{
  return FTL::StrRef( lhs ) == rhs;
}

inline bool operator!=( char const *lhs, FTL::StrRef rhs )
{
  return FTL::StrRef( lhs ) != rhs;
}

inline std::string &operator+=( std::string &stdString, FTL::StrRef str )
{
  return stdString.append( str.begin(), str.end() );
}

inline std::ostream &operator<<( std::ostream &os, FTL::StrRef str )
{
  for ( FTL::StrRef::IT it = str.begin(); it != str.end(); ++it )
    os << *it;
  return os;
}

#define FTL_STR(x) (::FTL::CStrRef( (x), (sizeof(x) - 1) ))

#endif //_FTL_StrRef_h
