#include <FTL/JSONValue.h>

#include <fstream>
#include <iostream>
#include <string>

static void check( bool cond )
{
  if ( ! cond )
  {
    std::cerr << "Assertion failed\n";
    exit( 1 );
  }
}

int main( int argc, char **argv )
{
  std::ifstream inputFile( "test.json" ); 
  std::string inputString(
    ( std::istreambuf_iterator<char>( inputFile ) ),
    ( std::istreambuf_iterator<char>() )
    );


  for ( size_t k = 0; k < 100; ++k )
  {
    FTL::StrRef str( inputString );
    FTL::JSONStr jsonStr( str );
    FTL::OwnedPtr<FTL::JSONObject> value(
      FTL::JSONValue::Decode( jsonStr )->cast<FTL::JSONObject>()
      );
  }
  
  // for ( size_t i = 0; i < 1000; ++i )
  // {
  //   char buf[32];
  //   snprintf( buf, 32, "array%lu", i );
  //   FTL::JSONArray const *array = value->getArray( buf );
  //   for ( size_t j = 0; j < 200; ++j )
  //   {
  //     FTL::JSONObject const *obj = array->getObject( j );
  //     check( obj->getFloat64( "width" ) == 10.0 ); 
  //     check( obj->getFloat64( "height" ) == 16.0 ); 
  //     check( obj->getString( "text" ) == FTL_STR("Hello World") ); 
  //   }
  // }

  return 0;
}
