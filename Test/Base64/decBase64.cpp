/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include <FTL/Base64Dec.h>

#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

#include <unistd.h>

int main( int argc, char **argv )
{
  std::string outputString;
  {
    FTL::Base64Dec<std::string> base64Dec( outputString );
    for ( int argi = 1; argi < argc; ++argi )
    {
      std::ifstream inputFile( argv[argi] ); 
      std::string inputString(
        (std::istreambuf_iterator<char>( inputFile )),
        (std::istreambuf_iterator<char>())
        );
      base64Dec << inputString;
    }
  }
  std::cout << outputString;
  return 0;
}
