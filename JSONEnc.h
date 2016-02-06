/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/ArrayRef.h>
#include <FTL/JSONException.h>
#include <FTL/JSONFormat.h>

#include <stdio.h>
#include <string>

//
// Usage:
//
// std::string string;
// {
//   JSONEnc enc( string );
//   JSONObjectEnc objectEnc( enc );
//   JSONEnc memberEnc( objectEnc, FTL_STR("key") );
//   JSONArrayEnc arrayEnc( memberEnc );
//   JSONEnc elementEnc( arrayEnc );
//   JSONSInt32Enc( elementEnc, 42 );
// }
//

FTL_NAMESPACE_BEGIN

template<typename StringTy = std::string>
class JSONElementEnc;

template<typename StringTy = std::string>
class JSONNullEnc;

template<typename StringTy = std::string>
class JSONBooleanEnc;

template<typename StringTy = std::string>
class JSONSInt32Enc;

template<typename StringTy = std::string>
class JSONFloat64Enc;

template<typename StringTy = std::string>
class JSONStringEnc;

template<typename StringTy = std::string>
class JSONListEnc;

template<typename StringTy = std::string>
class JSONObjectEnc;

template<typename StringTy = std::string>
class JSONArrayEnc;

template<typename StringTy = std::string>
class JSONEnc
{
  friend class JSONElementEnc<StringTy>;
  friend class JSONNullEnc<StringTy>;
  friend class JSONBooleanEnc<StringTy>;
  friend class JSONSInt32Enc<StringTy>;
  friend class JSONFloat64Enc<StringTy>;
  friend class JSONStringEnc<StringTy>;
  friend class JSONListEnc<StringTy>;
  friend class JSONObjectEnc<StringTy>;
  friend class JSONArrayEnc<StringTy>;

  JSONEnc( JSONEnc const &that );
  JSONEnc &operator=( JSONEnc const &that );

public:

  JSONEnc(
    StringTy &string,
    JSONFormat const &format = JSONFormat::Pretty()
    )
    : m_string( string )
    , m_format( format )
    , m_indents( 0 )
    , m_used( false )
  {
  }

  JSONEnc( JSONObjectEnc<StringTy> &objectEnc, StrRef key )
    : m_string( objectEnc.getEnc().m_string )
    , m_format( objectEnc.getEnc().m_format )
    , m_indents( objectEnc.getEnc().m_indents + 1 )
    , m_used( false )
  {
    objectEnc.inc();
    appendQuotedStrs( key, StrRef() );
    append( m_format.memberSepStr );
  }

  JSONEnc( JSONArrayEnc<StringTy> &arrayEnc )
    : m_string( arrayEnc.getEnc().m_string )
    , m_format( arrayEnc.getEnc().m_format )
    , m_indents( arrayEnc.getEnc().m_indents + 1 )
    , m_used( false )
  {
    arrayEnc.inc();
  }

  ~JSONEnc() {}

protected:

  void use()
  {
    if ( m_used )
      throw JSONEncodingErrorException();
    m_used = true;
  }

  JSONFormat const &getFormat() const
    { return m_format; }

  void reserve( size_t size )
    { m_string.reserve( m_string.size() + size ); }

  void append( char ch )
    { m_string += ch; }

  void append( StrRef str )
    { m_string += str; }

  void appendQuotedStrs(
    ArrayRef<StrRef> strs,
    StrRef delim
    )
  {
    size_t delimCount = strs.empty()? 0: strs.size() - 1;
    size_t expandAmount = 2 + delim.size() * delimCount;
    for ( ArrayRef<StrRef>::IT it = strs.begin(); it != strs.end(); ++it )
      expandAmount += 2 * it->size();
    reserve( expandAmount );

    append( '"' );
    const ArrayRef<StrRef>::IT itBegin = strs.begin();
    const ArrayRef<StrRef>::IT itEnd = strs.end();
    for ( ArrayRef<StrRef>::IT it = itBegin; it != itEnd; ++it )
    {
      if ( it != itBegin )
        appendQuotedStrChars( delim );
      appendQuotedStrChars( *it );
    }
    append( '"' );
  }

  void appendQuotedStrChars( StrRef str )
  {
    const StrRef::IT itEnd = str.end();
    for ( StrRef::IT it = str.begin(); it != itEnd; ++it )
    {
      switch ( *it )
      {
        case '\b':
          append( '\\' );
          append( 'b' );
          break;
        case '\f':
          append( '\\' );
          append( 'f' );
          break;
        case '\n':
          append( '\\' );
          append( 'n' );
          break;
        case '\r':
          append( '\\' );
          append( 'r' );
          break;
        case '\t':
          append( '\\' );
          append( 't' );
          break;
        case '"':
        case '\\':
          append( '\\' );
          // fall through
        default:
          append( *it );
          break;
      }
    }
  }

  void indent()
  {
    reserve(
        getFormat().newlineStr.size()
      + m_indents * getFormat().indentStr.size()
      );
    append( getFormat().newlineStr );
    for ( uint32_t indent = 0; indent <= m_indents; ++indent )
      append( getFormat().indentStr );
  }

private:

  StringTy &m_string;
  JSONFormat const &m_format;
  uint32_t const m_indents;
  bool m_used; 
};

template<typename StringTy>
class JSONElementEnc
{
  JSONElementEnc( JSONElementEnc const &that );
  JSONElementEnc &operator=( JSONElementEnc const &that );

protected:

  JSONElementEnc( JSONEnc<StringTy> &enc )
    { enc.use(); }
};

template<typename StringTy>
class JSONNullEnc : public JSONElementEnc<StringTy>
{
public:

  JSONNullEnc( JSONEnc<StringTy> &enc )
    : JSONElementEnc<StringTy>( enc )
    { enc.append( FTL_STR("null") ); }
};

template<typename StringTy>
class JSONBooleanEnc : public JSONElementEnc<StringTy>
{
public:

  JSONBooleanEnc( 
    JSONEnc<StringTy> &enc,
    bool value
    )
    : JSONElementEnc<StringTy>( enc )
    { enc.append( value? FTL_STR("true"): FTL_STR("false") ); }
};

template<typename StringTy>
class JSONSInt32Enc : public JSONElementEnc<StringTy>
{
public:

  JSONSInt32Enc( 
    JSONEnc<StringTy> &enc,
    int32_t value
    )
    : JSONElementEnc<StringTy>( enc )
  {
    char buf[24];
    int len = snprintf( buf, 24, "%d", value );
    enc.append( StrRef( buf, len ) );
  }
};

template<typename StringTy>
class JSONFloat64Enc : public JSONElementEnc<StringTy>
{
public:

  JSONFloat64Enc( 
    JSONEnc<StringTy> &enc,
    double value
    )
    : JSONElementEnc<StringTy>( enc )
  {
    char buf[32];
    char const *oldlocale = setlocale( LC_NUMERIC, "C" );
    int len = snprintf( buf, 24, "%.16lg", value );
    if ( oldlocale )
      setlocale( LC_NUMERIC, oldlocale );
    char *p = buf;
    bool haveDec = false;
    while ( *p )
    {
      if ( *p == '.' || *p == 'e' )
      {
        haveDec = true;
        break;
      }
      ++p;
    }
    if ( !haveDec )
    {
      buf[len++] = '.';
      buf[len++] = '0';
      buf[len] = '\0';
    }
    enc.append( StrRef( buf, len ) );
  }
};

template<typename StringTy>
class JSONStringEnc : public JSONElementEnc<StringTy>
{
public:

  JSONStringEnc( 
    JSONEnc<StringTy> &enc,
    ArrayRef<StrRef> strs,
    StrRef delim = StrRef()
    )
    : JSONElementEnc<StringTy>( enc )
    { enc.appendQuotedStrs( strs, delim ); }
};

template<typename StringTy>
class JSONListEnc : public JSONElementEnc<StringTy>
{
  friend class JSONEnc<StringTy>;

protected:

  JSONListEnc( JSONEnc<StringTy> &enc )
    : JSONElementEnc<StringTy>( enc )
    , m_enc( enc )
    , m_count( 0 )
    {}

  JSONEnc<StringTy> &getEnc()
    { return m_enc; }

  void inc()
  {
    if ( m_count++ > 0 )
      m_enc.append( m_enc.getFormat().elementSepStr );
    m_enc.indent();
  }

  void fin()
  {
    if ( m_count > 0 )
      m_enc.indent();
  }

private:

  JSONEnc<StringTy> &m_enc;
  uint32_t m_count;
};

template<typename StringTy>
class JSONObjectEnc : public JSONListEnc<StringTy>
{
public:

  JSONObjectEnc( JSONEnc<StringTy> &enc )
    : JSONListEnc<StringTy>( enc )
  {
    enc.append( enc.getFormat().objectBeginStr );
  }

  ~JSONObjectEnc()
  {
    this->fin();
    JSONEnc<StringTy> &enc = this->getEnc();
    enc.append( enc.getFormat().objectEndStr );
  }
};

template<typename StringTy>
class JSONArrayEnc : public JSONListEnc<StringTy>
{
public:

  JSONArrayEnc( JSONEnc<StringTy> &enc )
    : JSONListEnc<StringTy>( enc )
  {
    enc.append( enc.getFormat().arrayBeginStr );
  }

  ~JSONArrayEnc()
  {
    this->fin();
    JSONEnc<StringTy> &enc = this->getEnc();
    enc.append( enc.getFormat().arrayEndStr );
  }
};

FTL_NAMESPACE_END
