#include <FTL/SmallString.h>

#include <iostream>

void testSmallString()
{
  FTL::SmallString<16> s1, s2;
  std::cout << s1 << std::endl;

  for ( size_t i = 0; i < 64; ++i )
  {
    s1 += 'a';
    std::cout << s1 << std::endl;
  }

  s1.clear();
  std::cout << s1 << std::endl;

  s1 = FTL_STR("shortOne");
  s2 = FTL_STR("shortTwo");
  std::cout << FTL_STR("before") << std::endl;
  std::cout << s1 << std::endl;
  std::cout << s2 << std::endl;
  s1.swap( s2 );
  std::cout << FTL_STR("after") << std::endl;
  std::cout << s1 << std::endl;
  std::cout << s2 << std::endl;

  s1 = FTL_STR("shortOne");
  s2 = FTL_STR("longTwoLongTwoLongTwoLongTwo");
  std::cout << FTL_STR("before") << std::endl;
  std::cout << s1 << std::endl;
  std::cout << s2 << std::endl;
  s1.swap( s2 );
  std::cout << FTL_STR("after") << std::endl;
  std::cout << s1 << std::endl;
  std::cout << s2 << std::endl;

  s1 = FTL_STR("longOneLongOneLongOneLongOne");
  s2 = FTL_STR("shortTwo");
  std::cout << FTL_STR("before") << std::endl;
  std::cout << s1 << std::endl;
  std::cout << s2 << std::endl;
  s1.swap( s2 );
  std::cout << FTL_STR("after") << std::endl;
  std::cout << s1 << std::endl;
  std::cout << s2 << std::endl;

  s1 = FTL_STR("longOneLongOneLongOneLongOne");
  s2 = FTL_STR("longTwoLongTwoLongTwoLongTwo");
  std::cout << FTL_STR("before") << std::endl;
  std::cout << s1 << std::endl;
  std::cout << s2 << std::endl;
  s1.swap( s2 );
  std::cout << FTL_STR("after") << std::endl;
  std::cout << s1 << std::endl;
  std::cout << s2 << std::endl;

  s1 += "SomeSuffix";
  std::cout << s1 << std::endl;
  
  s1 = FTL_STR("One");
  std::cout << FTL_STR("c_str(): ") << s1.c_str() << std::endl;
  s1 += FTL_STR("TwoToMakeItLong");
  std::cout << FTL_STR("c_str(): ") << s1.c_str() << std::endl;
}

int main( int argc, char **argv )
{
  // sleep( 20 );
  // sleep( 5 );

  testSmallString();
}
