/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_JSONDecoder_h
#define _FTL_JSONDecoder_h

#include <FTL/Config.h>
#include <FTL/StrRef.h>

#include <algorithm>
#include <assert.h>
#include <stdint.h>

FTL_NAMESPACE_BEGIN

static const uint32_t JSONDecoderShortStringMaxLength = 16;

class JSONObjectDecoder;
class JSONArrayDecoder;

class JSONEntity
{
  friend class JSONObjectDecoder;
  friend class JSONArrayDecoder;

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

  static bool Consume(
    StrRef &str,
    uint32_t &line,
    uint32_t &column,
    char const *&error,
    JSONEntity &entity
    );

  JSONEntity()
    : type( Type_Undefined ) {}

  Type getType() const
    { return type; }

  StrRef getRawStr() const
    { return rawStr; }

  uint32_t getLine() const
    { return line; }

  uint32_t getColumn() const
    { return column; }

  void copyFrom( JSONEntity const &that )
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
        std::min( value.string.length, JSONDecoderShortStringMaxLength )
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

  JSONEntity( JSONEntity const &that )
    { copyFrom( that ); }

  JSONEntity &operator=( JSONEntity const &that )
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
    return stringLength() <= JSONDecoderShortStringMaxLength;
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
      std::min( str.size(), size_t(JSONDecoderShortStringMaxLength) )
      ) != 0 )
      return false;
    if ( value.string.length > JSONDecoderShortStringMaxLength )
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

  static bool ConsumeHex(
    StrRef &str,
    uint32_t &line,
    uint32_t &column,
    char const *&error,
    uint8_t &value
    );

  static bool ConsumeUCS2(
    StrRef &str,
    uint32_t &line,
    uint32_t &column,
    char const *&error,
    uint16_t &ucs2
    );

  static void StringAppendASCII( char ch, JSONEntity &entity );

  static uint8_t UCS2ToUTF8( uint16_t ucs2, char utf8[3] );

  static void StringAppendUCS2(
    uint16_t ucs2,
    JSONEntity &entity
    );

  static bool ConsumeColon(
    StrRef &str,
    uint32_t &line,
    uint32_t &column,
    char const *&error
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
      char shortData[JSONDecoderShortStringMaxLength];
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

inline void JSONEntity::stringGetData_Long( char *data ) const
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
          char const *error = 0;
          uint16_t ucs2;
          ConsumeUCS2( str, line, column, error, ucs2 );
          assert( !error );
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

inline bool JSONEntity::stringIs_Long( StrRef thatStr ) const
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
        char const *error = 0;
        uint16_t ucs2;
        ConsumeUCS2( str, line, column, error, ucs2 );
        assert( !error );

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

inline bool JSONEntity::ConsumeHex(
  StrRef &str,
  uint32_t &line,
  uint32_t &column,
  char const *&error,
  uint8_t &value
  )
{
  if ( str.empty() )
  {
    error = "expected hex digit";
    return false;
  }
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
      error = "invalid hex digit";
      return false;
  }
  str = str.drop_front();
  return true;
}

inline bool JSONEntity::ConsumeUCS2(
  StrRef &str,
  uint32_t &line,
  uint32_t &column,
  char const *&error,
  uint16_t &ucs2
  )
{
  uint8_t hex;
  if ( !ConsumeHex( str, line, column, error, hex ) ) return false;
  ucs2 = uint16_t(hex) << 12;
  if ( !ConsumeHex( str, line, column, error, hex ) ) return false;
  ucs2 |= uint16_t(hex) << 8;
  if ( !ConsumeHex( str, line, column, error, hex ) ) return false;
  ucs2 |= uint16_t(hex) << 4;
  if ( !ConsumeHex( str, line, column, error, hex ) ) return false;
  ucs2 |= uint16_t(hex);
  return true;
}

inline void JSONEntity::StringAppendASCII( char ch, JSONEntity &entity )
{
  if ( entity.value.string.length < JSONDecoderShortStringMaxLength )
    entity.value.string.shortData[entity.value.string.length] = ch;
  ++entity.value.string.length;
}

inline uint8_t JSONEntity::UCS2ToUTF8( uint16_t ucs2, char utf8[3] )
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

inline void JSONEntity::StringAppendUCS2(
  uint16_t ucs2,
  JSONEntity &entity
  )
{
  char utf8[3] = {0, 0, 0};
  uint32_t utf8Length = UCS2ToUTF8( ucs2, utf8 );
  StringAppendASCII( utf8[0], entity );
  if ( utf8Length > 1 )
  {
    StringAppendASCII( utf8[1], entity );
    if ( utf8Length > 2 )
      StringAppendASCII( utf8[2], entity );
  }
}

inline bool JSONEntity::ConsumeColon(
  StrRef &str,
  uint32_t &line,
  uint32_t &column,
  char const *&error
  )
{
  SkipWhitespace( str, line, column );
  if ( str.empty() || str[0] != ':' )
  {
    error = "expected ':'";
    return false;
  }
  str = str.drop_front();
  ++column;
  return true;
}

inline bool JSONEntity::Consume(
  StrRef &str,
  uint32_t &line,
  uint32_t &column,
  char const *&error,
  JSONEntity &entity
  )
{
  SkipWhitespace( str, line, column );
  if ( str.empty() )
    return false;

  entity.rawStr = str;
  entity.line = line;
  entity.column = column;
  switch ( str.front() )
  {
    case 'n':
    {
      if ( str.size() < 4
        || str[1] != 'u'
        || str[2] != 'l'
        || str[3] != 'l'
        )
      {
        error = "expected 'null'";
        return false;
      }
      str = str.drop_front( 4 );
      column += 4;
      entity.type = JSONEntity::Type_Null;
    }
    break;

    case 't':
    {
      if ( str.size() < 4
        || str[1] != 'r'
        || str[2] != 'u'
        || str[3] != 'e'
        )
      {
        error = "expected 'true'";
        return false;
      }
      str = str.drop_front( 4 );
      column += 4;
      entity.type = JSONEntity::Type_Boolean;
      entity.value.boolean = true;
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
      {
        error = "expected 'false'";
        return false;
      }
      str = str.drop_front( 5 );
      column += 5;
      entity.type = JSONEntity::Type_Boolean;
      entity.value.boolean = false;
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
      if ( !str.empty() && str.front() == '-' )
      {
        str = str.drop_front();
        ++column;
        if ( str.empty() )
        {
          error = "expected decimal digit";
          return false;
        }
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
          error = "expected decimal digit";
          return false;
      }

      if ( str.empty()
        || (str.front() != '.' && str.front() != 'e' && str.front() != 'E') )
      {
        entity.type = JSONEntity::Type_Int32;

        static const uint32_t maxIntegerLength = 15;
        uint32_t length = str.data() - entity.rawStr.data();
        if ( length > maxIntegerLength )
        {
          error = "integer too long";
          return false;
        }
        char buf[maxIntegerLength+1];
        memcpy( buf, entity.rawStr.data(), length );
        buf[length] = '\0';
        entity.value.int32 = atoi( buf );
      }
      else
      {
        entity.type = JSONEntity::Type_Float64;

        if ( str.front() == '.' )
        {
          str = str.drop_front();
          ++column;

          if ( str.empty() || str.front() < '0' || str.front() > '9' )
          {
            error = "expected decimal digit";
            return false;
          }
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
          {
            error = "expected decimal digit";
            return false;
          }
          str = str.drop_front();
          ++column;
          while ( !str.empty() && str.front() >= '0' && str.front() <= '9' )
          {
            str = str.drop_front();
            ++column;
          }
        }

        static const uint32_t maxScalarLength = 31;
        uint32_t length = str.data() - entity.rawStr.data();
        if ( length > maxScalarLength )
        {
          error = "floating point too long";
          return false;
        }
        char buf[maxScalarLength+1];
        memcpy( buf, entity.rawStr.data(), length );
        buf[length] = '\0';

        char const *oldlocale = setlocale( LC_NUMERIC, "C" );
        entity.value.float64 = atof( buf );
        if ( oldlocale )
          setlocale( LC_NUMERIC, oldlocale );
      }
    }
    break;

    case '"':
    case '\'':
    {
      entity.type = JSONEntity::Type_String;
      entity.value.string.length = 0;
      char quoteChar = str.front();
      str = str.drop_front();
      ++column;

      bool done = false;
      while ( !done )
      {
        if ( str.empty() )
        {
          error = "unterminated string";
          return false;
        }
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
          {
            error = "unterminated string";
            return false;
          }
          switch ( str.front() )
          {
            case '"':
            case '\'':
            case '/':
            case '\\':
              StringAppendASCII( str.front(), entity );
              str = str.drop_front();
              ++column;
              break;

            case 'b':
              StringAppendASCII( '\b', entity );
              str = str.drop_front();
              ++column;
              break;
            case 'f':
              StringAppendASCII( '\f', entity );
              str = str.drop_front();
              ++column;
              break;
            case 'n':
              StringAppendASCII( '\n', entity );
              str = str.drop_front();
              ++column;
              break;
            case 'r':
              StringAppendASCII( '\r', entity );
              str = str.drop_front();
              ++column;
              break;
            case 't':
              StringAppendASCII( '\t', entity );
              str = str.drop_front();
              ++column;
              break;

            case 'u':
              str = str.drop_front();
              ++column;
              uint16_t ucs2;
              if ( !ConsumeUCS2( str, line, column, error, ucs2 ) )
                return false;
              StringAppendUCS2( ucs2, entity );
              break;

            default:
              error = "invalid string escape character";
              return false;
          }
        }
        else
        {
          StringAppendASCII( str.front(), entity );
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
    }
    break;

    case '{':
    {
      entity.type = JSONEntity::Type_Object;
      entity.value.object.size = 0;
      str = str.drop_front();
      ++column;

      bool done = false;
      while ( !done )
      {
        SkipWhitespace( str, line, column );
        if ( str.empty() )
        {
          error = "expected entity or '}'";
          return false;
        }
        switch ( str.front() )
        {
          case '}':
            done = true;
            str = str.drop_front();
            ++column;
            break;

          default:
          {
            JSONEntity keyEntity;
            uint32_t keyLine = line;
            uint32_t keyColumn = column;
            if ( !Consume( str, line, column, error, keyEntity ) )
              return false;
            if ( !keyEntity.isString() )
            {
              line = keyLine;
              column = keyColumn;
              error = "expected string";
              return false;
            }

            if ( !ConsumeColon( str, line, column, error ) )
              return false;

            JSONEntity valueEntity;
            if ( !Consume( str, line, column, error, valueEntity ) )
              return false;

            ++entity.value.object.size;

            break;
          }
        }
      }
    }
    break;

    case '[':
    {
      entity.type = JSONEntity::Type_Array;
      entity.value.array.size = 0;
      str = str.drop_front();
      ++column;

      bool done = false;
      while ( !done )
      {
        SkipWhitespace( str, line, column );
        if ( str.empty() )
        {
          error = "expected entity or ']'";
          return false;
        }
        switch ( str.front() )
        {
          case ']':
            done = true;
            str = str.drop_front();
            ++column;
            break;

          default:
          {
            JSONEntity elementEntity;
            if ( !Consume( str, line, column, error, elementEntity ) )
              return false;

            ++entity.value.array.size;

            break;
          }
        }
      }
    }
    break;

    default:
      error = "unexpected character";
      return false;
  }

  entity.rawStr = entity.rawStr.drop_back( str.size() );
  return true;
}

inline void JSONEntity::SkipWhitespace(
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

class JSONDecoderBase
{
public:

  JSONDecoderBase( StrRef str )
    : m_str( str )
    , m_line( 1 )
    , m_column( 1 )
    , m_error( 0 )
  {
  }

  JSONDecoderBase( JSONEntity const &entity )
    : m_str( entity.getRawStr() )
    , m_line( entity.getLine() )
    , m_column( entity.getColumn() )
    , m_error( 0 )
  {
  }

  bool hadError() const
    { return !!m_error; }

  uint32_t getErrorLine() const
    { return m_line; }

  uint32_t getErrorColumn() const
    { return m_column; }

  char const *getErrorDesc() const
    { return m_error; }

protected:

  StrRef m_str;
  uint32_t m_line;
  uint32_t m_column;
  char const *m_error;
};

class JSONDecoder : public JSONDecoderBase
{
public:

  JSONDecoder( StrRef str )
    : JSONDecoderBase( str ) {}

  bool getNext(
    JSONEntity &entity
    )
  {
    return JSONEntity::Consume( m_str, m_line, m_column, m_error, entity );
  }
};

class JSONObjectDecoder : public JSONDecoderBase
{
public:

  JSONObjectDecoder( JSONEntity const &entity )
    : JSONDecoderBase( entity )
    , m_lastKeyShortData( 0 )
    , m_lastKeyLength( 0 )
  {
    assert( entity.isObject() );
    assert( !m_str.empty() );
    assert( m_str[0] == '{' );
    m_str = m_str.drop_front();
    ++m_column;
  }

  bool getNext(
    JSONEntity &key,
    JSONEntity &value
    )
  {
    JSONEntity::SkipWhitespace( m_str, m_line, m_column );

    if ( m_str.empty() || m_str[0] == '}' )
      return false;

    uint32_t keyLine = m_line;
    uint32_t keyColumn = m_column;
    if ( !JSONEntity::Consume( m_str, m_line, m_column, m_error, key ) )
      return false;
    if ( !key.isString() )
    {
      m_line = keyLine;
      m_column = keyColumn;
      m_error = "expected string";
      return false;
    }
    m_lastKeyShortData = key.value.string.shortData;
    m_lastKeyLength = key.value.string.length;

    if ( !JSONEntity::ConsumeColon( m_str, m_line, m_column, m_error ) )
      return false;

    if ( !JSONEntity::Consume( m_str, m_line, m_column, m_error, value ) )
      return false;

    return true;
  }

private:

  char const *m_lastKeyShortData;
  uint32_t m_lastKeyLength;
};

class JSONArrayDecoder : public JSONDecoderBase
{
public:

  JSONArrayDecoder( JSONEntity const &entity )
    : JSONDecoderBase( entity )
    , m_count( 0 )
    , m_lastIndex( 0 )
  {
    assert( entity.isArray() );
    assert( !m_str.empty() );
    assert( m_str[0] == '[' );
    m_str = m_str.drop_front();
    ++m_column;
  }

  bool getNext(
    JSONEntity &element
    )
  {
    JSONEntity::SkipWhitespace( m_str, m_line, m_column );
    
    if ( m_str.empty() || m_str[0] == ']' )
      return false;

    if ( !JSONEntity::Consume( m_str, m_line, m_column, m_error, element ) )
      return false;

    m_lastIndex = ++m_count;

    return true;
  }

private:

  uint32_t m_count;
  uint32_t m_lastIndex;
};

FTL_NAMESPACE_END

#endif //_FTL_JSONDecoder_h
