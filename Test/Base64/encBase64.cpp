/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include <FTL/Base64Enc.h>

#include <fstream>
#include <iostream>
#include <string>

#include <unistd.h>

int main( int argc, char **argv )
{
  std::string outputString;
  {
    FTL::Base64Enc<std::string> base64Enc( outputString );
    for ( int argi = 1; argi < argc; ++argi )
    {
      std::ifstream inputFile( argv[argi] ); 
      std::string inputString(
        ( std::istreambuf_iterator<char>( inputFile ) ),
        ( std::istreambuf_iterator<char>() )
        );
      base64Enc << inputString;
    }
  }
  std::cout << outputString << '\n';
}
