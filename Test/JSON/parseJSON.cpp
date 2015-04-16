/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FTL/JSONDecoder.h>

#include <iostream>
#include <string>

template<typename StringTy>
void AppendQuotedString(
  FTL::StrRef str,
  StringTy &string,
  uint32_t maxLength = UINT32_MAX,
  char quote = '\''
  )
{
  if ( quote )
    string += quote;
  FTL::StrRef::IT const itBegin = str.begin();
  FTL::StrRef::IT const itEnd = str.end();
  for ( FTL::StrRef::IT it = itBegin; it != itEnd; ++it )
  {
    if ( it - itBegin > maxLength )
    {
      string += "...";
      break;
    }
    else
    {
      char ch = *it;
      switch ( ch )
      {
        case '\\': string += "\\\\"; break;
        case '\0': string += "\\0"; break;
        case '\n': string += "\\n"; break;
        case '\r': string += "\\r"; break;
        case '\b': string += "\\b"; break;
        case '\t': string += "\\t"; break;
        case '\f': string += "\\f"; break;
        case '\a': string += "\\a"; break;
        case '\v': string += "\\v"; break;
        default:
          if ( quote && ch == quote )
          {
            string += '\\';
            string += quote;
          }
          else if ( (uint8_t)ch >= (uint8_t)0x80 // utf8
            || isprint((uint8_t)ch) )
            string += ch;
          else
          {
            string += "\\x";
            char const *hexDigits = "0123456789ABCDEF";
            string += hexDigits[uint8_t(ch)>>4];
            string += hexDigits[uint8_t(ch)&0x0F];
          }
          break;
      }
    }
  }
  if ( quote )
    string += quote;
}

void displayEntity(
  FTL::JSONEntity const &entity,
  std::string const &indent
  )
{
  std::cout << indent;
  std::cout << entity.getLine() << ':' << entity.getColumn() << ' ';
  switch ( entity.getType() )
  {
    case FTL::JSONEntity::Type_Null:
      std::cout << "NULL\n";
      break;
    case FTL::JSONEntity::Type_Boolean:
      std::cout << "BOOLEAN ";
      std::cout << (entity.booleanValue()? "true": "false");
      std::cout << '\n';
      break;
    case FTL::JSONEntity::Type_Int32:
    {
      std::cout << "INTEGER ";
      std::cout << entity.int32Value();
      std::cout << '\n';
    }
    break;
    case FTL::JSONEntity::Type_Float64:
    {
      std::cout << "SCALAR ";
      std::cout << entity.float64Value();
      std::cout << '\n';
      break;
    }
    case FTL::JSONEntity::Type_String:
    {
      std::string string;
      entity.stringAppendTo( string );

      std::string quotedString;
      AppendQuotedString( string, quotedString );

      std::cout << "STRING ";
      std::cout << entity.stringLength();
      std::cout << " ";
      std::cout << quotedString;
      std::cout << '\n';
      break;
    }
    case FTL::JSONEntity::Type_Object:
    {
      std::cout << "OBJECT " << entity.objectSize() << '\n';
      FTL::JSONObjectDecoder jod( entity );
      FTL::JSONEntity key, value;
      while ( jod.getNext( key, value ) )
      {
        displayEntity( key, "  " + indent );
        displayEntity( value, "    " + indent );
      }
    }
    break;
    case FTL::JSONEntity::Type_Array:
    {
      std::cout << "ARRAY " << entity.arraySize() << '\n';
      FTL::JSONArrayDecoder jad( entity );
      FTL::JSONEntity element;
      while ( jad.getNext( element ) )
        displayEntity( element, "  " + indent );
    }
    break;

    default:
      assert( false );
      break;
  }
}

void parseJSON( FILE *fp )
{
  static const uint32_t maxLength = 16*1024*1024;
  char *data = new char[maxLength];
  uint32_t length = 0;
  while ( !feof( fp ) )
  {
    int read = fread( &data[length], 1, maxLength - length, fp );
    if ( read <= 0 )
      break;
    length += read;
  }
  data[length] = '\0';

  FTL::JSONDecoder decoder( data );
  FTL::JSONEntity entity;
  while ( decoder.getNext( entity ) )
    displayEntity( entity, "" );
  if ( decoder.hadError() )
    printf(
      "Caught exception: malformed JSON (line %u, column %u): %s\n",
      decoder.getErrorLine(),
      decoder.getErrorColumn(),
      decoder.getErrorDesc()
      );

  delete [] data;
}

int main( int argc, char **argv )
{
  if ( argc == 1 )
    parseJSON( stdin );
  else
  {
    for ( int i=1; i<argc; ++i )
    {
      FILE *fp = fopen( argv[i], "r" );
      if ( !fp )
        perror( argv[i] );
      parseJSON( fp );
      fclose( fp );
    }
  }
  return 0;
}
