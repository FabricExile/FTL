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
    StrRef &str,
    uint32_t &line,
    uint32_t &column
    );

  static void ConsumeEntity(
    StrRef &str,
    uint32_t &line,
    uint32_t &column,
    JSONEnt &ent
    );

  static void ConsumeString(
    StrRef &str,
    uint32_t &line,
    uint32_t &column,
    JSONEnt &ent
    );

  static void ConsumeHex(
    StrRef &str,
    uint32_t &line,
    uint32_t &column,
    uint8_t &value
    );

  static void ConsumeUCS2(
    StrRef &str,
    uint32_t &line,
    uint32_t &column,
    uint16_t &ucs2
    );

  static void StringAppendASCII( char ch, JSONEnt &ent );

  static uint8_t UCS2ToUTF8( uint16_t ucs2, char utf8[3] );

  static void StringAppendUCS2(
    uint16_t ucs2,
    JSONEnt &ent
    );

  static void ConsumeColon(
    StrRef &str,
    uint32_t &line,
    uint32_t &column
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
          uint32_t line = 0, column = 0;
          uint16_t ucs2;
          ConsumeUCS2( str, line, column, ucs2 );
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
        uint32_t line = 0, column = 0;
        uint16_t ucs2;
        ConsumeUCS2( str, line, column, ucs2 );

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
  StrRef &str,
  uint32_t &line,
  uint32_t &column,
  uint8_t &value
  )
{
  if ( str.empty() )
    throw JSONMalformedException( line, column, FTL_STR("expected hex digit") );

  switch ( str.front() )
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
      throw JSONMalformedException( line, column, FTL_STR("invalid hex digit") );
  }
  str = str.drop_front();
}

inline void JSONEnt::ConsumeUCS2(
  StrRef &str,
  uint32_t &line,
  uint32_t &column,
  uint16_t &ucs2
  )
{
  uint8_t hex;
  ConsumeHex( str, line, column, hex );
  ucs2 = uint16_t(hex) << 12;
  ConsumeHex( str, line, column, hex );
  ucs2 |= uint16_t(hex) << 8;
  ConsumeHex( str, line, column, hex );
  ucs2 |= uint16_t(hex) << 4;
  ConsumeHex( str, line, column, hex );
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
  StrRef &str,
  uint32_t &line,
  uint32_t &column
  )
{
  if ( str.empty() || str[0] != ':' )
    throw JSONMalformedException( line, column, FTL_STR("expected ':'") );
  str = str.drop_front();
  ++column;
}

inline void JSONEnt::ConsumeString(
  StrRef &str,
  uint32_t &line,
  uint32_t &column,
  JSONEnt &ent
  )
{
  if ( str.empty() )
    throw JSONMalformedException( line, column, FTL_STR("expected string") );

  char quoteChar = str.front();
  if ( quoteChar != '"' && quoteChar != '\'' )
    throw JSONMalformedException( line, column, FTL_STR("expected string") );

  ent.type = JSONEnt::Type_String;
  ent.rawStr = str;
  ent.line = line;
  ent.column = column;
  ent.value.string.length = 0;

  str = str.drop_front();
  ++column;

  bool done = false;
  while ( !done )
  {
    if ( str.empty() )
      throw JSONMalformedException( line, column, FTL_STR("unterminated string") );

    if ( str.front() == quoteChar )
    {
      str = str.drop_front();
      ++column;
      done = true;
    }
    else if ( str.front() == '\\' )
    {
      str = str.drop_front();
      ++column;
      if ( str.empty() )
        throw JSONMalformedException( line, column, FTL_STR("unterminated string") );

      switch ( str.front() )
      {
        case '"':
        case '\'':
        case '/':
        case '\\':
          StringAppendASCII( str.front(), ent );
          str = str.drop_front();
          ++column;
          break;

        case 'b':
          StringAppendASCII( '\b', ent );
          str = str.drop_front();
          ++column;
          break;
        case 'f':
          StringAppendASCII( '\f', ent );
          str = str.drop_front();
          ++column;
          break;
        case 'n':
          StringAppendASCII( '\n', ent );
          str = str.drop_front();
          ++column;
          break;
        case 'r':
          StringAppendASCII( '\r', ent );
          str = str.drop_front();
          ++column;
          break;
        case 't':
          StringAppendASCII( '\t', ent );
          str = str.drop_front();
          ++column;
          break;

        case 'u':
          str = str.drop_front();
          ++column;
          uint16_t ucs2;
          ConsumeUCS2( str, line, column, ucs2 );
          StringAppendUCS2( ucs2, ent );
          break;

        default:
          throw JSONMalformedException( line, column, FTL_STR("invalid string escape character") );
      }
    }
    else
    {
      StringAppendASCII( str.front(), ent );
      switch ( str.front() )
      {
        case '\n':
          ++line;
          column = 1;
          break;

        case '\r':
          break;

        default:
        {
          uint8_t top = uint8_t(str.front()) & 0xC0;
          // utf-8
          if ( top != 0x80 )
            ++column;
        }
        break;
      }
      str = str.drop_front();
    }
  }

  ent.rawStr = ent.rawStr.drop_back( str.size() );
}

inline void JSONEnt::ConsumeEntity(
  StrRef &str,
  uint32_t &line,
  uint32_t &column,
  JSONEnt &ent
  )
{
  if ( str.empty() )
    throw JSONMalformedException( line, column, FTL_STR("expected ent") );

  switch ( str.front() )
  {
    case 'n':
    {
      if ( str.size() < 4
        || str[1] != 'u'
        || str[2] != 'l'
        || str[3] != 'l'
        )
        throw JSONMalformedException( line, column, FTL_STR("expected 'null'") );

      ent.type = JSONEnt::Type_Null;
      ent.rawStr = str;
      ent.line = line;
      ent.column = column;

      str = str.drop_front( 4 );
      column += 4;

      ent.rawStr = ent.rawStr.drop_back( str.size() );
    }
    break;

    case 't':
    {
      if ( str.size() < 4
        || str[1] != 'r'
        || str[2] != 'u'
        || str[3] != 'e'
        )
        throw JSONMalformedException( line, column, FTL_STR("expected 'true'") );

      ent.type = JSONEnt::Type_Boolean;
      ent.rawStr = str;
      ent.line = line;
      ent.column = column;

      ent.value.boolean = true;
      str = str.drop_front( 4 );
      column += 4;
      
      ent.rawStr = ent.rawStr.drop_back( str.size() );
    }
    break;

    case 'f':
    {
      if ( str.size() < 5
        || str[1] != 'a'
        || str[2] != 'l'
        || str[3] != 's'
        || str[4] != 'e'
        )
        throw JSONMalformedException( line, column, FTL_STR("expected 'false'") );

      ent.type = JSONEnt::Type_Boolean;
      ent.rawStr = str;
      ent.line = line;
      ent.column = column;
      ent.value.boolean = false;

      str = str.drop_front( 5 );
      column += 5;
      
      ent.rawStr = ent.rawStr.drop_back( str.size() );
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
      ent.rawStr = str;
      ent.line = line;
      ent.column = column;

      if ( !str.empty() && str.front() == '-' )
      {
        str = str.drop_front();
        ++column;
        if ( str.empty() )
          throw JSONMalformedException( line, column, FTL_STR("expected decimal digit") );
      }

      switch ( str.front() )
      {
        case '0':
          str = str.drop_front();
          ++column;
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
          str = str.drop_front();
          ++column;
          while ( !str.empty() && str.front() >= '0' && str.front() <= '9' )
          {
            str = str.drop_front();
            ++column;
          }
          break;
        default:
          throw JSONMalformedException( line, column, FTL_STR("expected decimal digit") );
      }

      if ( str.empty()
        || (str.front() != '.' && str.front() != 'e' && str.front() != 'E') )
      {
        ent.type = JSONEnt::Type_Int32;

        static const uint32_t maxIntegerLength = 15;
        uint32_t length = str.data() - ent.rawStr.data();
        if ( length > maxIntegerLength )
          throw JSONMalformedException( line, column, FTL_STR("integer too long") );

        char buf[maxIntegerLength+1];
        memcpy( buf, ent.rawStr.data(), length );
        buf[length] = '\0';
        ent.value.int32 = atoi( buf );
      }
      else
      {
        ent.type = JSONEnt::Type_Float64;

        if ( str.front() == '.' )
        {
          str = str.drop_front();
          ++column;

          if ( str.empty() || str.front() < '0' || str.front() > '9' )
            throw JSONMalformedException( line, column, FTL_STR("expected decimal digit") );

          while ( !str.empty() && str.front() >= '0' && str.front() <= '9' )
          {
            str = str.drop_front();
            ++column;
          }
        }

        if ( !str.empty() && (str.front() == 'e' || str.front() == 'E') )
        {
          str = str.drop_front();
          ++column;

          if ( !str.empty() && (str.front() == '-' || str.front() == '+') )
          {
            str = str.drop_front();
            ++column;
          }

          if ( str.empty() || str.front() < '0' || str.front() > '9' )
            throw JSONMalformedException( line, column, FTL_STR("expected decimal digit") );

          str = str.drop_front();
          ++column;
          while ( !str.empty() && str.front() >= '0' && str.front() <= '9' )
          {
            str = str.drop_front();
            ++column;
          }
        }

        static const uint32_t maxScalarLength = 31;
        uint32_t length = str.data() - ent.rawStr.data();
        if ( length > maxScalarLength )
          throw JSONMalformedException( line, column, FTL_STR("floating point too long") );

        char buf[maxScalarLength+1];
        memcpy( buf, ent.rawStr.data(), length );
        buf[length] = '\0';

        char const *oldlocale = setlocale( LC_NUMERIC, "C" );
        ent.value.float64 = atof( buf );
        if ( oldlocale )
          setlocale( LC_NUMERIC, oldlocale );
      }
      
      ent.rawStr = ent.rawStr.drop_back( str.size() );
    }
    break;

    case '"':
    case '\'':
      ConsumeString( str, line, column, ent );
      break;

    case '{':
    {
      ent.type = JSONEnt::Type_Object;
      ent.rawStr = str;
      ent.line = line;
      ent.column = column;
      ent.value.object.size = 0;

      str = str.drop_front();
      ++column;

      bool done = false;
      while ( !done )
      {
        SkipWhitespace( str, line, column );
        if ( str.empty() )
          throw JSONMalformedException( line, column, FTL_STR("expected string or '}'") );

        switch ( str.front() )
        {
          case '}':
            done = true;
            str = str.drop_front();
            ++column;
            break;

          default:
          {
            JSONEnt keyEntity;
            ConsumeString( str, line, column, keyEntity );

            SkipWhitespace( str, line, column );
            ConsumeColon( str, line, column );
            SkipWhitespace( str, line, column );

            JSONEnt valueEntity;
            ConsumeEntity( str, line, column, valueEntity );

            ++ent.value.object.size;

            break;
          }
        }
      }
      
      ent.rawStr = ent.rawStr.drop_back( str.size() );
    }
    break;

    case '[':
    {
      ent.type = JSONEnt::Type_Array;
      ent.rawStr = str;
      ent.line = line;
      ent.column = column;
      ent.value.array.size = 0;

      str = str.drop_front();
      ++column;

      bool done = false;
      while ( !done )
      {
        SkipWhitespace( str, line, column );
        if ( str.empty() )
          throw JSONMalformedException( line, column, FTL_STR("expected ent or ']'") );

        switch ( str.front() )
        {
          case ']':
            done = true;
            str = str.drop_front();
            ++column;
            break;

          default:
          {
            JSONEnt elementEntity;
            ConsumeEntity( str, line, column, elementEntity );

            ++ent.value.array.size;

            break;
          }
        }
      }
      
      ent.rawStr = ent.rawStr.drop_back( str.size() );
    }
    break;

    default:
      throw JSONMalformedException( line, column, FTL_STR("unrecognized character") );
  }
}

inline void JSONEnt::SkipWhitespace(
  StrRef &str,
  uint32_t &line,
  uint32_t &column
  )
{
  while ( !str.empty() )
  {
    switch ( str[0] )
    {
      case ' ':
      case '\t':
      case '\v':
      case '\f':
      case ',':
        str = str.drop_front();
        ++column;
        break;

      case '\r':
        str = str.drop_front();
        break;

      case '\n':
        str = str.drop_front();
        ++line;
        column = 1;
        break;

      case '/':
      {
        if ( str.size() == 1 )
          return;
        switch ( str[1] )
        {
          case '/':
            str = str.drop_front( 2 );
            column += 2;
            while ( !str.empty() && str.front() != '\n' )
            {
              str = str.drop_front();
              ++column;
            }
            break;

          case '*':
          {
            str = str.drop_front( 2 );
            column += 2;

            bool finished = false;
            while ( !finished && !str.empty() )
            {
              switch ( str.front() )
              {
                case '*':
                  if ( str.size() > 1 && str[1] == '/' )
                  {
                    str = str.drop_front();
                    ++column;
                    finished = true;
                  }
                  // fall through
                default:
                  str = str.drop_front();
                  ++column;
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

class JSONDecBase
{
public:

  JSONDecBase( StrRef &str, uint32_t &line, uint32_t &column )
    : m_str( str )
    , m_line( line )
    , m_column( column )
  {
  }

protected:

  StrRef &m_str;
  uint32_t &m_line;
  uint32_t &m_column;
};

class JSONDec : public JSONDecBase
{
public:

  JSONDec( StrRef &str, uint32_t &line, uint32_t &column )
    : JSONDecBase( str, line, column ) {}

  bool getNext( JSONEnt &ent )
  {
    JSONEnt::SkipWhitespace( m_str, m_line, m_column );
    if ( m_str.empty() )
      return false;

    JSONEnt::ConsumeEntity( m_str, m_line, m_column, ent );

    return true;
  }
};

class JSONObjectDec : public JSONDecBase
{
public:

  JSONObjectDec( StrRef &str, uint32_t &line, uint32_t &column )
    : JSONDecBase( str, line, column )
    , m_lastKeyShortData( 0 )
    , m_lastKeyLength( 0 )
  {
    JSONEnt::SkipWhitespace( m_str, m_line, m_column );
    if ( m_str.empty() || m_str.front() != '{' )
      throw JSONMalformedException( m_line, m_column, FTL_STR("expected '{'") );
    m_str = m_str.drop_front();
    ++m_column;
  }

  bool getNext( JSONEnt &key, JSONEnt &value )
  {
    JSONEnt::SkipWhitespace( m_str, m_line, m_column );
    if ( m_str.empty() || m_str.front() == '}' )
      return false;

    JSONEnt::ConsumeString( m_str, m_line, m_column, key );

    m_lastKeyShortData = key.value.string.shortData;
    m_lastKeyLength = key.value.string.length;

    JSONEnt::SkipWhitespace( m_str, m_line, m_column );
    JSONEnt::ConsumeColon( m_str, m_line, m_column );
    JSONEnt::SkipWhitespace( m_str, m_line, m_column );

    JSONEnt::ConsumeEntity( m_str, m_line, m_column, value );

    return true;
  }

private:

  char const *m_lastKeyShortData;
  uint32_t m_lastKeyLength;
};

class JSONArrayDec : public JSONDecBase
{
public:

  JSONArrayDec( StrRef &str, uint32_t &line, uint32_t &column )
    : JSONDecBase( str, line, column )
    , m_count( 0 )
    , m_lastIndex( 0 )
  {
    JSONEnt::SkipWhitespace( m_str, m_line, m_column );
    if ( m_str.empty() || m_str.front() != '[' )
      throw JSONMalformedException( m_line, m_column, FTL_STR("expected '['") );
    m_str = m_str.drop_front();
    ++m_column;
  }

  bool getNext( JSONEnt &element )
  {
    JSONEnt::SkipWhitespace( m_str, m_line, m_column );
    if ( m_str.empty() || m_str.front() == ']' )
      return false;

    JSONEnt::ConsumeEntity( m_str, m_line, m_column, element );

    m_lastIndex = ++m_count;

    return true;
  }

  uint32_t getLastIndex() const
    { return m_lastIndex; }

private:

  uint32_t m_count;
  uint32_t m_lastIndex;
};

FTL_NAMESPACE_END

#endif //_FTL_JSONDec_h
