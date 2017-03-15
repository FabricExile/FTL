/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include <FTL/JSONDec.h>

#include <iostream>
#include <string>
#include <stdint.h>
#include <vector>

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

void displayEnt(
  FTL::JSONEnt<FTL::JSONStrWithLoc> const &ent,
  std::string const &indent
  )
{
  std::cout << indent;
  std::cout << ent.getLine() << ':' << ent.getColumn() << ' ';
  switch ( ent.getType() )
  {
    case FTL::JSONEnt<FTL::JSONStrWithLoc>::Type_Null:
      std::cout << FTL_STR("NULL\n");
      break;
    case FTL::JSONEnt<FTL::JSONStrWithLoc>::Type_Boolean:
      std::cout << FTL_STR("BOOLEAN ");
      std::cout << (ent.booleanValue()? "true": "false");
      std::cout << '\n';
      break;
    case FTL::JSONEnt<FTL::JSONStrWithLoc>::Type_Int32:
    {
      std::cout << FTL_STR("INTEGER ");
      std::cout << ent.int32Value();
      std::cout << '\n';
    }
    break;
    case FTL::JSONEnt<FTL::JSONStrWithLoc>::Type_Float64:
    {
      std::cout << FTL_STR("SCALAR ");
      std::cout << ent.float64Value();
      std::cout << '\n';
      break;
    }
    case FTL::JSONEnt<FTL::JSONStrWithLoc>::Type_String:
    {
      std::string string;
      ent.stringAppendTo( string );

      std::string quotedString;
      AppendQuotedString( string, quotedString );

      std::cout << FTL_STR("STRING ");
      std::cout << ent.stringLength();
      std::cout << " ";
      std::cout << quotedString;
      std::cout << '\n';
      break;
    }
    case FTL::JSONEnt<FTL::JSONStrWithLoc>::Type_Object:
    {
      std::cout << FTL_STR("OBJECT ") << ent.objectSize() << '\n';
      FTL::JSONStrWithLoc ds( ent.getRawJSONStr() );
      FTL::JSONObjectDec<FTL::JSONStrWithLoc> objectDec( ds );
      FTL::JSONEnt<FTL::JSONStrWithLoc> key, value;
      while ( objectDec.getNext( key, value ) )
      {
        displayEnt( key, "  " + indent );
        displayEnt( value, "    " + indent );
      }
    }
    break;
    case FTL::JSONEnt<FTL::JSONStrWithLoc>::Type_Array:
    {
      std::cout << FTL_STR("ARRAY ") << ent.arraySize() << '\n';
      FTL::JSONStrWithLoc ds( ent.getRawJSONStr() );
      FTL::JSONArrayDec<FTL::JSONStrWithLoc> arrayDec( ds );
      FTL::JSONEnt<FTL::JSONStrWithLoc> element;
      while ( arrayDec.getNext( element ) )
        displayEnt( element, "  " + indent );
    }
    break;

    default:
      assert( false );
      break;
  }
}

void parseJSON( FILE *fp )
{
  static const size_t MaxRead = 16*1024;
  std::vector<char> jsonInput;
  while ( !feof( fp ) )
  {
    size_t oldSize = jsonInput.size();
    jsonInput.resize( oldSize + MaxRead );
    int read = fread(
      &jsonInput[oldSize],
      1,
      MaxRead,
      fp
      );
    if ( read <= 0 )
    {
      jsonInput.resize( oldSize );
      break;
    }
    jsonInput.resize( oldSize + read );
  }

  FTL::JSONStrWithLoc strWithLoc(
    FTL::StrRef( jsonInput.empty()? 0: &jsonInput[0], jsonInput.size() )
    );
  FTL::JSONDec<FTL::JSONStrWithLoc> decoder( strWithLoc );
  try
  {
    FTL::JSONEnt<FTL::JSONStrWithLoc> ent;
    while ( decoder.getNext( ent ) )
      displayEnt( ent, "" );
  }
  catch ( FTL::JSONException e )
  {
    std::cout
      << "Caught exception: "
      << e.getDesc()
      << "\n";
  }
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
