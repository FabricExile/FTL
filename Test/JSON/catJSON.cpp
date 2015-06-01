/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FTL/JSONValue.h>

#include <iostream>
#include <string>

void catJSON( FILE *fp )
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
  for (;;)
  {
    try
    {
      FTL::OwnedPtr<FTL::JSONValue> jsonValue(
        FTL::JSONValue::Decode( strWithLoc )
        );
      if ( !jsonValue )
        break;
      std::string string;
      jsonValue->encode( string );
      std::cout << string << '\n';
    }
    catch ( FTL::JSONException e )
    {
      std::cout
        << "Caught exception: "
        << e.getDesc()
        << "\n";
    }
  }
}

int main( int argc, char **argv )
{
  if ( argc == 1 )
    catJSON( stdin );
  else
  {
    for ( int i=1; i<argc; ++i )
    {
      FILE *fp = fopen( argv[i], "r" );
      if ( !fp )
        perror( argv[i] );
      catJSON( fp );
      fclose( fp );
    }
  }
  return 0;
}
