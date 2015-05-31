/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_JSONDec_h
#define _FTL_JSONDec_h

#include <FTL/Config.h>
#include <FTL/JSONException.h>
#include <FTL/StrRef.h>

#include <algorithm>
#include <assert.h>
#include <stdint.h>

FTL_NAMESPACE_BEGIN

static const uint32_t JSONDecShortStringMaxLength = 16;

struct JSONDecState
{
  StrRef str;
  uint32_t line, column;

  JSONDecState( StrRef theStr, uint32_t theLine = 1, uint32_t theColumn = 1 )
    : str( theStr ), line( theLine ), column( theColumn ) {}
};

class JSONDec;
class JSONObjectDec;
class JSONArrayDec;

class JSONEnt
{
  friend class JSONDec;
  friend class JSONObjectDec;
  friend class JSONArrayDec;

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

  StrRef getRawStr() const
    { return rawStr; }

  uint32_t getLine() const
    { return line; }

  uint32_t getColumn() const
    { return column; }

  void copyFrom( JSONEnt const &that )
  {
    rawStr = that.rawStr;
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
        std::min( value.string.length, JSONDecShortStringMaxLength )
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
      std::min( str.size(), size_t(JSONDecShortStringMaxLength) )
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
    JSONDecState &ds
    );

  static void ConsumeEntity(
    JSONDecState &ds,
    JSONEnt &ent
    );

  static void ConsumeString(
    JSONDecState &ds,
    JSONEnt &ent
    );

  static void ConsumeHex(
    JSONDecState &ds,
    uint8_t &value
    );

  static void ConsumeUCS2(
    JSONDecState &ds,
    uint16_t &ucs2
    );

  static void StringAppendASCII( char ch, JSONEnt &ent );

  static uint8_t UCS2ToUTF8( uint16_t ucs2, char utf8[3] );

  static void StringAppendUCS2(
    uint16_t ucs2,
    JSONEnt &ent
    );

  static void ConsumeColon(
    JSONDecState &ds
    );

private:

  Type type;
  StrRef rawStr;
  uint32_t line;
  uint32_t column;
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

inline void JSONEnt::stringGetData_Long( char *data ) const
{
  assert( isString() );

  StrRef str = rawStr;

  assert( !str.empty() );
  char quoteChar = str.front();
  assert( quoteChar == '"' || quoteChar == '\'' );
  str = str.drop_front();

  bool done = false;
  while ( !done )
  {
    assert( !str.empty() );

    if ( str.front()  == quoteChar )
      done = true;
    else if ( str.front() == '\\' )
    {
      str = str.drop_front();
      assert( !str.empty() );
      switch ( str.front() )
      {
        case '"':
        case '\'':
        case '/':
        case '\\':
          *data++ = str.front();
          str = str.drop_front();
          break;

        case 'b':
          *data++ = '\b';
          str = str.drop_front();
          break;
        case 'f':
          *data++ = '\f';
          str = str.drop_front();
          break;
        case 'n':
          *data++ = '\n';
          str = str.drop_front();
          break;
        case 'r':
          *data++ = '\r';
          str = str.drop_front();
          break;
        case 't':
          *data++ = '\t';
          str = str.drop_front();
          break;

        case 'u':
        {
          str = str.drop_front();
          JSONDecState ds( str );
          uint16_t ucs2;
          ConsumeUCS2( ds, ucs2 );
          data += UCS2ToUTF8( ucs2, data );
        }
        break;

        default:
          assert( false );
          str = str.drop_front();
          break;
      }
    }
    else
    {
      *data++ = str.front();
      str = str.drop_front();
    }
  }

  assert( str.size() == 1 );
  assert( str.front() == quoteChar );
}

inline bool JSONEnt::stringIs_Long( StrRef thatStr ) const
{
  assert( isString() );

  StrRef str = rawStr;

  assert( !str.empty() );
  char quoteChar = str.front();
  assert( quoteChar == '"' || quoteChar == '\'' );
  str = str.drop_front();

  while ( !thatStr.empty() )
  {
    assert( !str.empty() );
    if ( str.front() == quoteChar )
    {
      assert( false );
      return false;
    }
    else if ( str.front() == '\\' )
    {
      str = str.drop_front();
      assert( !str.empty() );
      switch ( str.front() )
      {
      case '"':
      case '\'':
      case '/':
      case '\\':
        if ( thatStr.front() != str.front() )
          return false;
        str = str.drop_front();
        thatStr = thatStr.drop_front();
        break;

      case 'b':
        if ( thatStr.front() != '\b' )
          return false;
        str = str.drop_front();
        thatStr = thatStr.drop_front();
        break;
      case 'f':
        if ( thatStr.front() != '\f' )
          return false;
        str = str.drop_front();
        thatStr = thatStr.drop_front();
        break;
      case 'n':
        if ( thatStr.front() != '\n' )
          return false;
        str = str.drop_front();
        thatStr = thatStr.drop_front();
        break;
      case 'r':
        if ( thatStr.front() != '\r' )
          return false;
        str = str.drop_front();
        thatStr = thatStr.drop_front();
        break;
      case 't':
        if ( thatStr.front() != '\t' )
          return false;
        str = str.drop_front();
        thatStr = thatStr.drop_front();
        break;

      case 'u':
      {
        str = str.drop_front();
        JSONDecState ds( str );
        uint16_t ucs2;
        ConsumeUCS2( ds, ucs2 );

        char utf8[3];
        uint32_t utf8Length = UCS2ToUTF8( ucs2, utf8 );
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
      if ( str.front() != thatStr.front() )
        return false;
      str = str.drop_front();
      thatStr = thatStr.drop_front();
    }
  }

  if ( str.size() == 1 )
  {
    assert( str.front() == quoteChar );
    return true;
  }
  else return false;
}

inline void JSONEnt::ConsumeHex(
  JSONDecState &ds,
  uint8_t &value
  )
{
  if ( ds.str.empty() )
    throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected hex digit") );

  switch ( ds.str.front() )
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
      throw JSONMalformedException( ds.line, ds.column, FTL_STR("invalid hex digit") );
  }
  ds.str = ds.str.drop_front();
}

inline void JSONEnt::ConsumeUCS2(
  JSONDecState &ds,
  uint16_t &ucs2
  )
{
  uint8_t hex;
  ConsumeHex( ds, hex );
  ucs2 = uint16_t(hex) << 12;
  ConsumeHex( ds, hex );
  ucs2 |= uint16_t(hex) << 8;
  ConsumeHex( ds, hex );
  ucs2 |= uint16_t(hex) << 4;
  ConsumeHex( ds, hex );
  ucs2 |= uint16_t(hex);
}

inline void JSONEnt::StringAppendASCII( char ch, JSONEnt &ent )
{
  if ( ent.value.string.length < JSONDecShortStringMaxLength )
    ent.value.string.shortData[ent.value.string.length] = ch;
  ++ent.value.string.length;
}

inline uint8_t JSONEnt::UCS2ToUTF8( uint16_t ucs2, char utf8[3] )
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

inline void JSONEnt::StringAppendUCS2(
  uint16_t ucs2,
  JSONEnt &ent
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

inline void JSONEnt::ConsumeColon(
  JSONDecState &ds
  )
{
  if ( ds.str.empty() || ds.str[0] != ':' )
    throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected ':'") );
  ds.str = ds.str.drop_front();
  ++ds.column;
}

inline void JSONEnt::ConsumeString(
  JSONDecState &ds,
  JSONEnt &ent
  )
{
  if ( ds.str.empty() )
    throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected string") );

  char quoteChar = ds.str.front();
  if ( quoteChar != '"' && quoteChar != '\'' )
    throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected string") );

  ent.type = JSONEnt::Type_String;
  ent.rawStr = ds.str;
  ent.line = ds.line;
  ent.column = ds.column;
  ent.value.string.length = 0;

  ds.str = ds.str.drop_front();
  ++ds.column;

  bool done = false;
  while ( !done )
  {
    if ( ds.str.empty() )
      throw JSONMalformedException( ds.line, ds.column, FTL_STR("unterminated string") );

    if ( ds.str.front() == quoteChar )
    {
      ds.str = ds.str.drop_front();
      ++ds.column;
      done = true;
    }
    else if ( ds.str.front() == '\\' )
    {
      ds.str = ds.str.drop_front();
      ++ds.column;
      if ( ds.str.empty() )
        throw JSONMalformedException( ds.line, ds.column, FTL_STR("unterminated string") );

      switch ( ds.str.front() )
      {
        case '"':
        case '\'':
        case '/':
        case '\\':
          StringAppendASCII( ds.str.front(), ent );
          ds.str = ds.str.drop_front();
          ++ds.column;
          break;

        case 'b':
          StringAppendASCII( '\b', ent );
          ds.str = ds.str.drop_front();
          ++ds.column;
          break;
        case 'f':
          StringAppendASCII( '\f', ent );
          ds.str = ds.str.drop_front();
          ++ds.column;
          break;
        case 'n':
          StringAppendASCII( '\n', ent );
          ds.str = ds.str.drop_front();
          ++ds.column;
          break;
        case 'r':
          StringAppendASCII( '\r', ent );
          ds.str = ds.str.drop_front();
          ++ds.column;
          break;
        case 't':
          StringAppendASCII( '\t', ent );
          ds.str = ds.str.drop_front();
          ++ds.column;
          break;

        case 'u':
          ds.str = ds.str.drop_front();
          ++ds.column;
          uint16_t ucs2;
          ConsumeUCS2( ds, ucs2 );
          StringAppendUCS2( ucs2, ent );
          break;

        default:
          throw JSONMalformedException( ds.line, ds.column, FTL_STR("invalid string escape character") );
      }
    }
    else
    {
      StringAppendASCII( ds.str.front(), ent );
      switch ( ds.str.front() )
      {
        case '\n':
          ++ds.line;
          ds.column = 1;
          break;

        case '\r':
          break;

        default:
        {
          uint8_t top = uint8_t(ds.str.front()) & 0xC0;
          // utf-8
          if ( top != 0x80 )
            ++ds.column;
        }
        break;
      }
      ds.str = ds.str.drop_front();
    }
  }

  ent.rawStr = ent.rawStr.drop_back( ds.str.size() );
}

inline void JSONEnt::ConsumeEntity(
  JSONDecState &ds,
  JSONEnt &ent
  )
{
  if ( ds.str.empty() )
    throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected ent") );

  switch ( ds.str.front() )
  {
    case 'n':
    {
      if ( ds.str.size() < 4
        || ds.str[1] != 'u'
        || ds.str[2] != 'l'
        || ds.str[3] != 'l'
        )
        throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected 'null'") );

      ent.type = JSONEnt::Type_Null;
      ent.rawStr = ds.str;
      ent.line = ds.line;
      ent.column = ds.column;

      ds.str = ds.str.drop_front( 4 );
      ds.column += 4;

      ent.rawStr = ent.rawStr.drop_back( ds.str.size() );
    }
    break;

    case 't':
    {
      if ( ds.str.size() < 4
        || ds.str[1] != 'r'
        || ds.str[2] != 'u'
        || ds.str[3] != 'e'
        )
        throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected 'true'") );

      ent.type = JSONEnt::Type_Boolean;
      ent.rawStr = ds.str;
      ent.line = ds.line;
      ent.column = ds.column;

      ent.value.boolean = true;
      ds.str = ds.str.drop_front( 4 );
      ds.column += 4;
      
      ent.rawStr = ent.rawStr.drop_back( ds.str.size() );
    }
    break;

    case 'f':
    {
      if ( ds.str.size() < 5
        || ds.str[1] != 'a'
        || ds.str[2] != 'l'
        || ds.str[3] != 's'
        || ds.str[4] != 'e'
        )
        throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected 'false'") );

      ent.type = JSONEnt::Type_Boolean;
      ent.rawStr = ds.str;
      ent.line = ds.line;
      ent.column = ds.column;
      ent.value.boolean = false;

      ds.str = ds.str.drop_front( 5 );
      ds.column += 5;
      
      ent.rawStr = ent.rawStr.drop_back( ds.str.size() );
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
      ent.rawStr = ds.str;
      ent.line = ds.line;
      ent.column = ds.column;

      if ( !ds.str.empty() && ds.str.front() == '-' )
      {
        ds.str = ds.str.drop_front();
        ++ds.column;
        if ( ds.str.empty() )
          throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected decimal digit") );
      }

      switch ( ds.str.front() )
      {
        case '0':
          ds.str = ds.str.drop_front();
          ++ds.column;
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
          ds.str = ds.str.drop_front();
          ++ds.column;
          while ( !ds.str.empty() && ds.str.front() >= '0' && ds.str.front() <= '9' )
          {
            ds.str = ds.str.drop_front();
            ++ds.column;
          }
          break;
        default:
          throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected decimal digit") );
      }

      if ( ds.str.empty()
        || (ds.str.front() != '.' && ds.str.front() != 'e' && ds.str.front() != 'E') )
      {
        ent.type = JSONEnt::Type_Int32;

        static const uint32_t maxIntegerLength = 15;
        uint32_t length = ds.str.data() - ent.rawStr.data();
        if ( length > maxIntegerLength )
          throw JSONMalformedException( ds.line, ds.column, FTL_STR("integer too long") );

        char buf[maxIntegerLength+1];
        memcpy( buf, ent.rawStr.data(), length );
        buf[length] = '\0';
        ent.value.int32 = atoi( buf );
      }
      else
      {
        ent.type = JSONEnt::Type_Float64;

        if ( ds.str.front() == '.' )
        {
          ds.str = ds.str.drop_front();
          ++ds.column;

          if ( ds.str.empty() || ds.str.front() < '0' || ds.str.front() > '9' )
            throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected decimal digit") );

          while ( !ds.str.empty() && ds.str.front() >= '0' && ds.str.front() <= '9' )
          {
            ds.str = ds.str.drop_front();
            ++ds.column;
          }
        }

        if ( !ds.str.empty() && (ds.str.front() == 'e' || ds.str.front() == 'E') )
        {
          ds.str = ds.str.drop_front();
          ++ds.column;

          if ( !ds.str.empty() && (ds.str.front() == '-' || ds.str.front() == '+') )
          {
            ds.str = ds.str.drop_front();
            ++ds.column;
          }

          if ( ds.str.empty() || ds.str.front() < '0' || ds.str.front() > '9' )
            throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected decimal digit") );

          ds.str = ds.str.drop_front();
          ++ds.column;
          while ( !ds.str.empty() && ds.str.front() >= '0' && ds.str.front() <= '9' )
          {
            ds.str = ds.str.drop_front();
            ++ds.column;
          }
        }

        static const uint32_t maxScalarLength = 31;
        uint32_t length = ds.str.data() - ent.rawStr.data();
        if ( length > maxScalarLength )
          throw JSONMalformedException( ds.line, ds.column, FTL_STR("floating point too long") );

        char buf[maxScalarLength+1];
        memcpy( buf, ent.rawStr.data(), length );
        buf[length] = '\0';

        char const *oldlocale = setlocale( LC_NUMERIC, "C" );
        ent.value.float64 = atof( buf );
        if ( oldlocale )
          setlocale( LC_NUMERIC, oldlocale );
      }
      
      ent.rawStr = ent.rawStr.drop_back( ds.str.size() );
    }
    break;

    case '"':
    case '\'':
      ConsumeString( ds, ent );
      break;

    case '{':
    {
      ent.type = JSONEnt::Type_Object;
      ent.rawStr = ds.str;
      ent.line = ds.line;
      ent.column = ds.column;
      ent.value.object.size = 0;

      ds.str = ds.str.drop_front();
      ++ds.column;

      bool done = false;
      while ( !done )
      {
        SkipWhitespace( ds );
        if ( ds.str.empty() )
          throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected string or '}'") );

        switch ( ds.str.front() )
        {
          case '}':
            done = true;
            ds.str = ds.str.drop_front();
            ++ds.column;
            break;

          default:
          {
            JSONEnt keyEntity;
            ConsumeString( ds, keyEntity );

            SkipWhitespace( ds );
            ConsumeColon( ds );
            SkipWhitespace( ds );

            JSONEnt valueEntity;
            ConsumeEntity( ds, valueEntity );

            ++ent.value.object.size;

            break;
          }
        }
      }
      
      ent.rawStr = ent.rawStr.drop_back( ds.str.size() );
    }
    break;

    case '[':
    {
      ent.type = JSONEnt::Type_Array;
      ent.rawStr = ds.str;
      ent.line = ds.line;
      ent.column = ds.column;
      ent.value.array.size = 0;

      ds.str = ds.str.drop_front();
      ++ds.column;

      bool done = false;
      while ( !done )
      {
        SkipWhitespace( ds );
        if ( ds.str.empty() )
          throw JSONMalformedException( ds.line, ds.column, FTL_STR("expected ent or ']'") );

        switch ( ds.str.front() )
        {
          case ']':
            done = true;
            ds.str = ds.str.drop_front();
            ++ds.column;
            break;

          default:
          {
            JSONEnt elementEntity;
            ConsumeEntity( ds, elementEntity );

            ++ent.value.array.size;

            break;
          }
        }
      }
      
      ent.rawStr = ent.rawStr.drop_back( ds.str.size() );
    }
    break;

    default:
      throw JSONMalformedException( ds.line, ds.column, FTL_STR("unrecognized character") );
  }
}

inline void JSONEnt::SkipWhitespace(
  JSONDecState &ds
  )
{
  while ( !ds.str.empty() )
  {
    switch ( ds.str[0] )
    {
      case ' ':
      case '\t':
      case '\v':
      case '\f':
      case ',':
        ds.str = ds.str.drop_front();
        ++ds.column;
        break;

      case '\r':
        ds.str = ds.str.drop_front();
        break;

      case '\n':
        ds.str = ds.str.drop_front();
        ++ds.line;
        ds.column = 1;
        break;

      case '/':
      {
        if ( ds.str.size() == 1 )
          return;
        switch ( ds.str[1] )
        {
          case '/':
            ds.str = ds.str.drop_front( 2 );
            ds.column += 2;
            while ( !ds.str.empty() && ds.str.front() != '\n' )
            {
              ds.str = ds.str.drop_front();
              ++ds.column;
            }
            break;

          case '*':
          {
            ds.str = ds.str.drop_front( 2 );
            ds.column += 2;

            bool finished = false;
            while ( !finished && !ds.str.empty() )
            {
              switch ( ds.str.front() )
              {
                case '*':
                  if ( ds.str.size() > 1 && ds.str[1] == '/' )
                  {
                    ds.str = ds.str.drop_front();
                    ++ds.column;
                    finished = true;
                  }
                  // fall through
                default:
                  ds.str = ds.str.drop_front();
                  ++ds.column;
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

class JSONDec
{
public:

  JSONDec( JSONDecState &ds )
    : m_ds( ds ) {}

  bool getNext( JSONEnt &ent )
  {
    JSONEnt::SkipWhitespace( m_ds );
    if ( m_ds.str.empty() )
      return false;

    JSONEnt::ConsumeEntity( m_ds, ent );

    return true;
  }

private:

  JSONDecState &m_ds;
};

class JSONObjectDec
{
public:

  JSONObjectDec( JSONDecState &ds )
    : m_ds( ds )
    , m_lastKeyShortData( 0 )
    , m_lastKeyLength( 0 )
  {
    JSONEnt::SkipWhitespace( m_ds );
    if ( m_ds.str.empty() || m_ds.str.front() != '{' )
      throw JSONMalformedException( m_ds.line, m_ds.column, FTL_STR("expected '{'") );
    m_ds.str = m_ds.str.drop_front();
    ++m_ds.column;
  }

  bool getNext( JSONEnt &key, JSONEnt &value )
  {
    JSONEnt::SkipWhitespace( m_ds );
    if ( m_ds.str.empty() || m_ds.str.front() == '}' )
      return false;

    JSONEnt::ConsumeString( m_ds, key );

    m_lastKeyShortData = key.value.string.shortData;
    m_lastKeyLength = key.value.string.length;

    JSONEnt::SkipWhitespace( m_ds );
    JSONEnt::ConsumeColon( m_ds );
    JSONEnt::SkipWhitespace( m_ds );

    JSONEnt::ConsumeEntity( m_ds, value );

    return true;
  }

private:

  JSONDecState &m_ds;
  char const *m_lastKeyShortData;
  uint32_t m_lastKeyLength;
};

class JSONArrayDec
{
public:

  JSONArrayDec( JSONDecState &ds )
    : m_ds( ds )
    , m_count( 0 )
    , m_lastIndex( 0 )
  {
    JSONEnt::SkipWhitespace( m_ds );
    if ( m_ds.str.empty() || m_ds.str.front() != '[' )
      throw JSONMalformedException( m_ds.line, m_ds.column, FTL_STR("expected '['") );
    m_ds.str = m_ds.str.drop_front();
    ++m_ds.column;
  }

  bool getNext( JSONEnt &element )
  {
    JSONEnt::SkipWhitespace( m_ds );
    if ( m_ds.str.empty() || m_ds.str.front() == ']' )
      return false;

    JSONEnt::ConsumeEntity( m_ds, element );

    m_lastIndex = ++m_count;

    return true;
  }

  uint32_t getLastIndex() const
    { return m_lastIndex; }

private:

  JSONDecState &m_ds;
  uint32_t m_count;
  uint32_t m_lastIndex;
};

FTL_NAMESPACE_END

#endif //_FTL_JSONDec_h
