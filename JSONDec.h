/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/JSONException.h>
#include <FTL/StrRef.h>

#include <algorithm>
#include <assert.h>
#include <math.h>
#include <stdint.h>

FTL_NAMESPACE_BEGIN

static const uint32_t JSONDecShortStringMaxLength = 16;

struct JSONStr
{
  JSONStr() {}

  JSONStr( StrRef str, uint32_t line = 0, uint32_t column = 0 )
    : m_str( str ), m_off( 0 ) {}

  bool empty() const
    { return m_off == m_str.size(); }

  size_t size() const
    { return m_str.size() - m_off; }

  char const *data() const
    { return m_str.data() + m_off; }

  char front() const
    { return m_str[m_off]; }

  char operator[]( size_t index ) const
    { return m_str[m_off + index]; }

  void drop( size_t count = 1 )
    { m_off += count; }

  void drop_back( size_t count )
    { m_str = m_str.drop_back( count ); }

  uint32_t getLine() const
    { return 0; }

  uint32_t getColumn() const
    { return 0; }

private:

  StrRef m_str;
  size_t m_off;
};

struct JSONStrWithLoc
{
  JSONStrWithLoc() {}

  JSONStrWithLoc( StrRef str, uint32_t line = 1, uint32_t column = 1 )
    : m_str( str ), m_off( 0 ), m_line( line ), m_column( column ) {}

  bool empty() const
    { return m_off == m_str.size(); }

  size_t size() const
    { return m_str.size() - m_off; }

  char const *data() const
    { return m_str.data() + m_off; }

  char front() const
    { return m_str[m_off]; }

  char operator[]( size_t index ) const
    { return m_str[m_off + index]; }

  void drop( size_t count = 1 )
  {
    while ( count > 0 )
    {
      switch ( m_str[m_off] )
      {
        case '\n':
          ++m_line;
          m_column = 1;
          break;

        case '\r':
          break;

        default:
        {
          uint8_t top = uint8_t(m_str[m_off]) & 0xC0;
          // utf-8
          if ( top != 0x80 )
            ++m_column;
        }
        break;
      }
      ++m_off;
      --count;
    }
  }

  void drop_back( size_t count )
    { m_str = m_str.drop_back( count ); }

  uint32_t getLine() const
    { return m_line; }

  uint32_t getColumn() const
    { return m_column; }

private:

  StrRef m_str;
  size_t m_off;
  uint32_t m_line, m_column;
};

template<typename JSONStrTy>
class JSONDec;

template<typename JSONStrTy>
class JSONObjectDec;

template<typename JSONStrTy>
class JSONArrayDec;

template<typename JSONStrTy>
class JSONEnt
{
  friend class JSONDec<JSONStrTy>;
  friend class JSONObjectDec<JSONStrTy>;
  friend class JSONArrayDec<JSONStrTy>;

public:

  enum Type
  {
    Type_Undefined,
    Type_Null,
    Type_Boolean,
    Type_Int32,
    Type_Float64,
    Type_String,
    Type_Object,
    Type_Array
  };

  JSONEnt()
    : type( Type_Undefined ) {}

  Type getType() const
    { return type; }

  JSONStrTy getRawJSONStr() const
    { return rawJSONStr; }

  uint32_t getLine() const
    { return rawJSONStr.getLine(); }

  uint32_t getColumn() const
    { return rawJSONStr.getColumn(); }

  void copyFrom( JSONEnt const &that )
  {
    rawJSONStr = that.rawJSONStr;
    switch ( (type = that.type) )
    {
    case Type_Boolean:
      value.boolean = that.value.boolean;
      break;
    case Type_Int32:
      value.int32 = that.value.int32;
      break;
    case Type_Float64:
      value.float64 = that.value.float64;
      break;
    case Type_String:
      value.string.length = that.value.string.length;
      memcpy(
        value.string.shortData,
        that.value.string.shortData,
        (std::min)( value.string.length, JSONDecShortStringMaxLength )
        );
      break;
    case Type_Object:
      value.object.size = that.value.object.size;
      break;
    case Type_Array:
      value.array.size = that.value.array.size;
      break;
    default:
      break;
    }
  }

  JSONEnt( JSONEnt const &that )
    { copyFrom( that ); }

  JSONEnt &operator=( JSONEnt const &that )
  {
    copyFrom( that );
    return *this;
  }

  // Undefined

  bool isUndefined() const
    { return type == Type_Undefined; }

  operator bool () const
    { return !isUndefined(); }

  bool operator !() const
    { return isUndefined(); }

  // Null

  bool isNull() const
    { return type == Type_Null; }

  // Boolean

  bool isBoolean() const
    { return type == Type_Boolean; }

  bool booleanValue() const
  {
    assert( isBoolean() );
    return value.boolean;
  }

  // Int32

  bool isInt32() const
    { return type == Type_Int32; }

  int32_t int32Value() const
  {
    assert( isInt32() );
    return value.int32;
  }

  // Scalar

  bool isFloat64() const
    { return type == Type_Float64; }

  double float64Value() const
  {
    assert( isFloat64() );
    return value.float64;
  }

  // String

  bool isString() const
    { return type == Type_String; }

  uint32_t stringLength() const
  {
    assert( isString() );
    return value.string.length;
  }

  bool stringIsShort() const
  {
    assert( isString() );
    return stringLength() <= JSONDecShortStringMaxLength;
  }

  char const *stringShortData() const
  {
    assert( stringIsShort() );
    return value.string.shortData;
  }

  void stringGetData( char *data ) const
  {
    assert( isString() );
    if ( stringIsShort() )
      memcpy( data, stringShortData(), stringLength() );
    else
      stringGetData_Long( data );
  }

  bool stringIs( StrRef str ) const
  {
    assert( isString() );
    if ( str.size() != value.string.length )
      return false;
    if ( memcmp(
      str.data(), value.string.shortData,
      (std::min)( str.size(), size_t(JSONDecShortStringMaxLength) )
      ) != 0 )
      return false;
    if ( value.string.length > JSONDecShortStringMaxLength )
      return stringIs_Long( str );
    else return true;
  }

  template<typename StringTy>
  void stringAppendTo( StringTy &string ) const
  {
    assert( isString() );
    uint32_t size = stringLength();
    if ( size > 0 )
    {
      uint32_t oldSize = string.size();
      uint32_t newSize = oldSize + size;
      string.resize( newSize );
      stringGetData( &string[oldSize] );
    }
  }

  // Object

  bool isObject() const
    { return type == Type_Object; }

  uint32_t objectSize() const
    { return value.object.size; }

  // Array

  bool isArray() const
    { return type == Type_Array; }

  uint32_t arraySize() const
    { return value.array.size; }

protected:

  void stringGetData_Long( char *data ) const;

  bool stringIs_Long( StrRef thatStr ) const;

  static void SkipWhitespace(
    JSONStrTy &ds
    );

  static void ConsumeEntity(
    JSONStrTy &ds,
    JSONEnt *ent
    );

  static void ConsumeString(
    JSONStrTy &ds,
    JSONEnt *ent
    );

  static uint8_t ConsumeHex( JSONStrTy &ds );

  static uint16_t ConsumeUCS2( JSONStrTy &ds );

  static void StringAppendASCII( char ch, JSONEnt &ent );

  static uint8_t UCS2ToUTF8( uint16_t ucs2, char utf8[3] );

  static void StringAppendUCS2(
    uint16_t ucs2,
    JSONEnt &ent
    );

  static void ConsumeColon(
    JSONStrTy &ds
    );

private:

  Type type;
  JSONStrTy rawJSONStr;
  union
  {
    bool boolean;
    int32_t int32;
    double float64;
    struct
    {
      uint32_t length;
      char shortData[JSONDecShortStringMaxLength];
    } string;
    struct
    {
      uint32_t size;
    } object;
    struct
    {
      uint32_t size;
    } array;
  } value;
};

template<typename JSONStrTy>
void JSONEnt<JSONStrTy>::stringGetData_Long( char *data ) const
{
  assert( isString() );

  JSONStrTy jsonStr( rawJSONStr );

  assert( !jsonStr.empty() );
  char quoteChar = jsonStr.front();
  assert( quoteChar == '"' || quoteChar == '\'' );
  jsonStr.drop();

  bool done = false;
  while ( !done )
  {
    assert( !jsonStr.empty() );

    if ( jsonStr.front()  == quoteChar )
      done = true;
    else if ( jsonStr.front() == '\\' )
    {
      jsonStr.drop();
      assert( !jsonStr.empty() );
      switch ( jsonStr.front() )
      {
        case '"':
        case '\'':
        case '/':
        case '\\':
          *data++ = jsonStr.front();
          jsonStr.drop();
          break;

        case 'b':
          *data++ = '\b';
          jsonStr.drop();
          break;
        case 'f':
          *data++ = '\f';
          jsonStr.drop();
          break;
        case 'n':
          *data++ = '\n';
          jsonStr.drop();
          break;
        case 'r':
          *data++ = '\r';
          jsonStr.drop();
          break;
        case 't':
          *data++ = '\t';
          jsonStr.drop();
          break;

        case 'u':
        {
          jsonStr.drop();
          data += UCS2ToUTF8( ConsumeUCS2( jsonStr ), data );
        }
        break;

        default:
          assert( false );
          jsonStr.drop();
          break;
      }
    }
    else
    {
      *data++ = jsonStr.front();
      jsonStr.drop();
    }
  }

  assert( jsonStr.size() == 1 );
  assert( jsonStr.front() == quoteChar );
}

template<typename JSONStrTy>
bool JSONEnt<JSONStrTy>::stringIs_Long( StrRef thatStr ) const
{
  assert( isString() );

  JSONStrTy jsonStr( rawJSONStr );

  assert( !jsonStr.empty() );
  char quoteChar = jsonStr.front();
  assert( quoteChar == '"' || quoteChar == '\'' );
  jsonStr.drop();

  while ( !thatStr.empty() )
  {
    assert( !jsonStr.empty() );
    if ( jsonStr.front() == quoteChar )
    {
      assert( false );
      return false;
    }
    else if ( jsonStr.front() == '\\' )
    {
      jsonStr.drop();
      assert( !jsonStr.empty() );
      switch ( jsonStr.front() )
      {
      case '"':
      case '\'':
      case '/':
      case '\\':
        if ( thatStr.front() != jsonStr.front() )
          return false;
        jsonStr.drop();
        thatStr = thatStr.drop_front();
        break;

      case 'b':
        if ( thatStr.front() != '\b' )
          return false;
        jsonStr.drop();
        thatStr = thatStr.drop_front();
        break;
      case 'f':
        if ( thatStr.front() != '\f' )
          return false;
        jsonStr.drop();
        thatStr = thatStr.drop_front();
        break;
      case 'n':
        if ( thatStr.front() != '\n' )
          return false;
        jsonStr.drop();
        thatStr = thatStr.drop_front();
        break;
      case 'r':
        if ( thatStr.front() != '\r' )
          return false;
        jsonStr.drop();
        thatStr = thatStr.drop_front();
        break;
      case 't':
        if ( thatStr.front() != '\t' )
          return false;
        jsonStr.drop();
        thatStr = thatStr.drop_front();
        break;

      case 'u':
      {
        jsonStr.drop();
        JSONStrTy ds( jsonStr );

        char utf8[3];
        uint32_t utf8Length = UCS2ToUTF8( ConsumeUCS2( ds ), utf8 );
        if ( thatStr.size() < utf8Length
          || memcmp( utf8, thatStr.data(), utf8Length ) != 0 )
          return false;
        thatStr = thatStr.drop_front( utf8Length );
      }
      break;

      default:
        assert( false );
        return false;
      }
    }
    else
    {
      if ( jsonStr.front() != thatStr.front() )
        return false;
      jsonStr.drop();
      thatStr = thatStr.drop_front();
    }
  }

  if ( jsonStr.size() == 1 )
  {
    assert( jsonStr.front() == quoteChar );
    return true;
  }
  else return false;
}

template<typename JSONStrTy>
uint8_t JSONEnt<JSONStrTy>::ConsumeHex( JSONStrTy &ds )
{
  if ( ds.empty() )
    throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected hex digit") );

  uint8_t value;
  switch ( ds.front() )
  {
    case '0': value = 0; break;
    case '1': value = 1; break;
    case '2': value = 2; break;
    case '3': value = 3; break;
    case '4': value = 4; break;
    case '5': value = 5; break;
    case '6': value = 6; break;
    case '7': value = 7; break;
    case '8': value = 8; break;
    case '9': value = 9; break;
    case 'a': case 'A': value = 10; break;
    case 'b': case 'B': value = 11; break;
    case 'c': case 'C': value = 12; break;
    case 'd': case 'D': value = 13; break;
    case 'e': case 'E': value = 14; break;
    case 'f': case 'F': value = 15; break;
    default:
      throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("invalid hex digit") );
  }
  ds.drop();
  return value;
}

template<typename JSONStrTy>
uint16_t JSONEnt<JSONStrTy>::ConsumeUCS2( JSONStrTy &ds )
{
  return
      ( uint16_t( ConsumeHex( ds ) ) << 12 )
    | ( uint16_t( ConsumeHex( ds ) ) << 8 )
    | ( uint16_t( ConsumeHex( ds ) ) << 4 )
    |   uint16_t( ConsumeHex( ds ) );
}

template<typename JSONStrTy>
void JSONEnt<JSONStrTy>::StringAppendASCII( char ch, JSONEnt &ent )
{
  if ( ent.value.string.length < JSONDecShortStringMaxLength )
    ent.value.string.shortData[ent.value.string.length] = ch;
  ++ent.value.string.length;
}

template<typename JSONStrTy>
uint8_t JSONEnt<JSONStrTy>::UCS2ToUTF8( uint16_t ucs2, char utf8[3] )
{
  if ( ucs2 < 0x80 )
  {
    utf8[0] = char(ucs2);
    return 1;
  }
  else if ( ucs2 < 0x800)
  {
    utf8[0] = char(ucs2 >> 6) | char(0xC0);
    utf8[1] = char(ucs2& 0x3F) | char(0x80);
    return 2;
  }
  else
  {
    utf8[0] = char(ucs2 >> 12) | char(0xE0);
    utf8[1] = (char(ucs2 >> 6) & char(0x3F)) | char(0xE0);
    utf8[2] = (char(ucs2) & char(0x3F)) | char(0x80);
    return 3;
  }
}

template<typename JSONStrTy>
void JSONEnt<JSONStrTy>::StringAppendUCS2(
  uint16_t ucs2,
  JSONEnt<JSONStrTy> &ent
  )
{
  char utf8[3] = {0, 0, 0};
  uint32_t utf8Length = UCS2ToUTF8( ucs2, utf8 );
  StringAppendASCII( utf8[0], ent );
  if ( utf8Length > 1 )
  {
    StringAppendASCII( utf8[1], ent );
    if ( utf8Length > 2 )
      StringAppendASCII( utf8[2], ent );
  }
}

template<typename JSONStrTy>
void JSONEnt<JSONStrTy>::ConsumeColon(
  JSONStrTy &ds
  )
{
  if ( ds.empty() || ds.front() != ':' )
    throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected ':'") );
  ds.drop();
}

template<typename JSONStrTy>
void JSONEnt<JSONStrTy>::ConsumeString(
  JSONStrTy &ds,
  JSONEnt<JSONStrTy> *ent
  )
{
  if ( ds.empty() )
    throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected string") );

  char quoteChar = ds.front();
  if ( quoteChar != '"' && quoteChar != '\'' )
    throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected string") );

  if ( ent )
  {
    ent->type = JSONEnt::Type_String;
    ent->rawJSONStr = ds;
    ent->value.string.length = 0;
  }

  ds.drop();

  bool done = false;
  while ( !done )
  {
    if ( ds.empty() )
      throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("unterminated string") );

    if ( ds.front() == quoteChar )
    {
      ds.drop();
      done = true;
    }
    else if ( ds.front() == '\\' )
    {
      ds.drop();
      if ( ds.empty() )
        throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("unterminated string") );

      switch ( ds.front() )
      {
        case '"':
        case '\'':
        case '/':
        case '\\':
          if ( ent )
            StringAppendASCII( ds.front(), *ent );
          ds.drop();
          break;

        case 'b':
          if ( ent )
            StringAppendASCII( '\b', *ent );
          ds.drop();
          break;
        case 'f':
          if ( ent )
            StringAppendASCII( '\f', *ent );
          ds.drop();
          break;
        case 'n':
          if ( ent )
            StringAppendASCII( '\n', *ent );
          ds.drop();
          break;
        case 'r':
          if ( ent )
            StringAppendASCII( '\r', *ent );
          ds.drop();
          break;
        case 't':
          if ( ent )
            StringAppendASCII( '\t', *ent );
          ds.drop();
          break;

        case 'u':
          ds.drop();
          if ( ent )
            StringAppendUCS2( ConsumeUCS2( ds ), *ent );
          break;

        default:
          throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("invalid string escape character") );
      }
    }
    else
    {
      if ( ent )
        StringAppendASCII( ds.front(), *ent );
      ds.drop();
    }
  }

  if ( ent )
    ent->rawJSONStr.drop_back( ds.size() );
}

template<typename JSONStrTy>
void JSONEnt<JSONStrTy>::ConsumeEntity(
  JSONStrTy &ds,
  JSONEnt<JSONStrTy> *ent
  )
{
  if ( ds.empty() )
    throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected entity") );

  switch ( ds.front() )
  {
    case 'n':
    {
      if ( ds.size() < 4
        || ds[1] != 'u'
        || ds[2] != 'l'
        || ds[3] != 'l'
        )
        throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected 'null'") );

      if ( ent )
      {
        ent->type = JSONEnt::Type_Null;
        ent->rawJSONStr = ds;
        ent->rawJSONStr.drop_back( ds.size() - 4 );
      }

      ds.drop( 4 );
    }
    break;

    case 't':
    {
      if ( ds.size() < 4
        || ds[1] != 'r'
        || ds[2] != 'u'
        || ds[3] != 'e'
        )
        throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected 'true'") );

      if ( ent )
      {
        ent->type = JSONEnt::Type_Boolean;
        ent->rawJSONStr = ds;
        ent->rawJSONStr.drop_back( ds.size() - 4 );
        ent->value.boolean = true;
      }

      ds.drop( 4 );
    }
    break;

    case 'f':
    {
      if ( ds.size() < 5
        || ds[1] != 'a'
        || ds[2] != 'l'
        || ds[3] != 's'
        || ds[4] != 'e'
        )
        throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected 'false'") );

      if ( ent )
      {
        ent->type = JSONEnt::Type_Boolean;
        ent->rawJSONStr = ds;
        ent->rawJSONStr.drop_back( ds.size() - 5 );
        ent->value.boolean = false;
      }

      ds.drop( 5 );
    }
    break;

    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
      if ( ent )
        ent->rawJSONStr = ds;

      bool mantIsNeg = 0;
      if ( !ds.empty() && ds.front() == '-' )
      {
        mantIsNeg = true;
        ds.drop();
        if ( ds.empty() )
          throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected decimal digit") );
      }

      int32_t whole;
      switch ( ds.front() )
      {
        case '0':
          whole = 0;
          ds.drop();
          break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          whole = ds.front() - '0';
          ds.drop();
          while ( !ds.empty() && ds.front() >= '0' && ds.front() <= '9' )
          {
            whole = 10 * whole + (ds.front() - '0');
            ds.drop();
          }
          break;
        default:
          throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected decimal digit") );
      }

      if ( ds.empty()
        || (ds.front() != '.' && ds.front() != 'e' && ds.front() != 'E') )
      {
        if ( ent )
        {
          if ( mantIsNeg )
            whole = -whole;
          ent->type = JSONEnt::Type_Int32;
          ent->value.int32 = whole;
        }
      }
      else
      {
        if ( ent )
          ent->type = JSONEnt::Type_Float64;

        int64_t mant = whole;
        int32_t expAdj = 0;
        if ( ds.front() == '.' )
        {
          ds.drop();

          if ( ds.empty() || ds.front() < '0' || ds.front() > '9' )
            throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected decimal digit") );

          while ( !ds.empty() && ds.front() >= '0' && ds.front() <= '9' )
          {
            mant = 10 * mant + (ds.front() - '0');
            --expAdj;
            ds.drop();
          }
        }

        int32_t exp = 0;
        if ( !ds.empty() && (ds.front() == 'e' || ds.front() == 'E') )
        {
          ds.drop();

          bool expIsNeg;
          if ( !ds.empty() && (ds.front() == '-' || ds.front() == '+') )
          {
            expIsNeg = ds.front() == '-';
            ds.drop();
          }
          else expIsNeg = false;

          if ( ds.empty() || ds.front() < '0' || ds.front() > '9' )
            throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected decimal digit") );

          exp = 10 * exp + (ds.front() - '0');
          ds.drop();
          while ( !ds.empty() && ds.front() >= '0' && ds.front() <= '9' )
          {
            exp = 10 * exp + (ds.front() - '0');
            ds.drop();
          }

          if ( expIsNeg )
            exp = -exp;
        }
        exp += expAdj;

        if ( ent )
        {
          if ( mantIsNeg )
            mant = -mant;
          ent->value.float64 = double(mant);
          if ( exp != 0 )
            ent->value.float64 *= pow( 10.0, double( exp ) );
        }
      }
      
      if ( ent )
        ent->rawJSONStr.drop_back( ds.size() );
    }
    break;

    case '"':
    case '\'':
      ConsumeString( ds, ent );
      break;

    case '{':
    {
      if ( ent )
      {
        ent->type = JSONEnt::Type_Object;
        ent->rawJSONStr = ds;
        ent->value.object.size = 0;
      }

      ds.drop();

      bool done = false;
      while ( !done )
      {
        SkipWhitespace( ds );
        if ( ds.empty() )
          throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected string or '}'") );

        switch ( ds.front() )
        {
          case '}':
            done = true;
            ds.drop();
            break;

          default:
          {
            ConsumeEntity( ds, 0 );

            SkipWhitespace( ds );
            ConsumeColon( ds );
            SkipWhitespace( ds );

            ConsumeEntity( ds, 0 );

            if ( ent )
              ++ent->value.object.size;

            break;
          }
        }
      }
      
      if ( ent )
        ent->rawJSONStr.drop_back( ds.size() );
    }
    break;

    case '[':
    {
      if ( ent )
      {
        ent->type = JSONEnt::Type_Array;
        ent->rawJSONStr = ds;
        ent->value.array.size = 0;
      }

      ds.drop();

      bool done = false;
      while ( !done )
      {
        SkipWhitespace( ds );
        if ( ds.empty() )
          throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("expected entity or ']'") );

        switch ( ds.front() )
        {
          case ']':
            done = true;
            ds.drop();
            break;

          default:
          {
            ConsumeEntity( ds, 0 );

            if ( ent )
              ++ent->value.array.size;

            break;
          }
        }
      }
      
      if ( ent )
        ent->rawJSONStr.drop_back( ds.size() );
    }
    break;

    default:
      throw JSONMalformedException( ds.getLine(), ds.getColumn(), FTL_STR("unrecognized character") );
  }
}

template<typename JSONStrTy>
void JSONEnt<JSONStrTy>::SkipWhitespace(
  JSONStrTy &ds
  )
{
  while ( !ds.empty() )
  {
    switch ( ds.front() )
    {
      case ' ':
      case '\t':
      case '\v':
      case '\f':
      case ',':
      case '\r':
      case '\n':
        ds.drop();
        break;

      case '/':
      {
        if ( ds.size() == 1 )
          return;
        switch ( ds[1] )
        {
          case '/':
            ds.drop( 2 );
            while ( !ds.empty() && ds.front() != '\n' )
            {
              ds.drop();
            }
            break;

          case '*':
          {
            ds.drop( 2 );

            bool finished = false;
            while ( !finished && !ds.empty() )
            {
              switch ( ds.front() )
              {
                case '*':
                  if ( ds.size() > 1 && ds[1] == '/' )
                  {
                    ds.drop();
                    finished = true;
                  }
                  // fall through
                default:
                  ds.drop();
                  break;
              }
            }
          }

          default: return;
        }
      }
      break;

      default: return;
    }
  }
}

template<typename JSONStrTy>
class JSONDec
{
public:

  JSONDec( JSONStrTy &ds )
    : m_ds( ds ) {}

  bool getNext( JSONEnt<JSONStrTy> &ent )
  {
    JSONEnt<JSONStrTy>::SkipWhitespace( m_ds );
    if ( m_ds.empty() )
      return false;

    JSONEnt<JSONStrTy>::ConsumeEntity( m_ds, &ent );

    return true;
  }

private:

  JSONStrTy &m_ds;
};

template<typename JSONStrTy>
class JSONObjectDec
{
public:

  JSONObjectDec( JSONStrTy &ds )
    : m_ds( ds )
  {
    JSONEnt<JSONStrTy>::SkipWhitespace( m_ds );
    if ( m_ds.empty() || m_ds.front() != '{' )
      throw JSONMalformedException( m_ds.getLine(), m_ds.getColumn(), FTL_STR("expected '{'") );
    m_ds.drop();
  }

  bool getNext( JSONEnt<JSONStrTy> &key, JSONEnt<JSONStrTy> &value )
  {
    JSONEnt<JSONStrTy>::SkipWhitespace( m_ds );
    if ( m_ds.empty() || m_ds.front() == '}' )
      return false;

    JSONEnt<JSONStrTy>::ConsumeString( m_ds, &key );

    JSONEnt<JSONStrTy>::SkipWhitespace( m_ds );
    JSONEnt<JSONStrTy>::ConsumeColon( m_ds );
    JSONEnt<JSONStrTy>::SkipWhitespace( m_ds );

    JSONEnt<JSONStrTy>::ConsumeEntity( m_ds, &value );

    return true;
  }

private:

  JSONStrTy &m_ds;
};

template<typename JSONStrTy>
class JSONArrayDec
{
public:

  JSONArrayDec( JSONStrTy &ds )
    : m_ds( ds )
  {
    JSONEnt<JSONStrTy>::SkipWhitespace( m_ds );
    if ( m_ds.empty() || m_ds.front() != '[' )
      throw JSONMalformedException( m_ds.getLine(), m_ds.getColumn(), FTL_STR("expected '['") );
    m_ds.drop();
  }

  bool getNext( JSONEnt<JSONStrTy> &element )
  {
    JSONEnt<JSONStrTy>::SkipWhitespace( m_ds );
    if ( m_ds.empty() || m_ds.front() == ']' )
      return false;

    JSONEnt<JSONStrTy>::ConsumeEntity( m_ds, &element );

    return true;
  }

private:

  JSONStrTy &m_ds;
};

FTL_NAMESPACE_END
