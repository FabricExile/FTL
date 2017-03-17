/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#include <FTL/OrderedStringMap.h>
#include <FTL/SmallString.h>

#include <exception>
#include <iostream>
#include <map>
#include <stdlib.h>

static const size_t maxAllocatedPtrs = 4096;
std::pair<void *, size_t> allocatedPtrs[maxAllocatedPtrs];
size_t allocatedPtrCount = 0;

void *operator new( std::size_t size ) throw(std::bad_alloc)
{
  assert( allocatedPtrCount < maxAllocatedPtrs );
  if ( size == 0 )
    throw std::bad_alloc();
  void *ptr = malloc( size );
  allocatedPtrs[allocatedPtrCount++] = std::pair<void *, size_t>( ptr, size );
  return ptr;
}

void operator delete( void *ptr ) throw()
{
  if ( ptr )
  {
    size_t allocatedPtrIndex = 0;
    while ( allocatedPtrIndex < allocatedPtrCount )
      if ( allocatedPtrs[allocatedPtrIndex++].first == ptr )
        break;
    assert( allocatedPtrIndex <= allocatedPtrCount );
    while ( allocatedPtrIndex < allocatedPtrCount )
    {
      allocatedPtrs[allocatedPtrIndex] = allocatedPtrs[allocatedPtrIndex+1];
      ++allocatedPtrIndex;
    }
    --allocatedPtrCount;

    free( ptr );
  }
}

void dumpAllocatedPtrs()
{
  std::cout << FTL_STR("AllocatedPtrs: Begin") << std::endl;
  for ( size_t allocatedPtrIndex = 0; allocatedPtrIndex < allocatedPtrCount; ++allocatedPtrIndex )
    std::cout << FTL_STR("  ptr ") << allocatedPtrs[allocatedPtrIndex].first << FTL_STR(" size ") << allocatedPtrs[allocatedPtrIndex].second << std::endl;
  std::cout << FTL_STR("AllocatedPtrs: End") << std::endl;
}

typedef FTL::OrderedStringMap< FTL::OrderedStringMap<int>::KeyTy > OSM;

OSM::KeyTy genString( size_t length )
{
  static const size_t Base64CharCount = 64;
  static const char Base64Chars[Base64CharCount + 1] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  assert( length <= Base64CharCount );
  return OSM::KeyTy( FTL::StrRef( Base64Chars, length ) );
}

void testSmallString()
{
  std::cout << FTL_STR("SmallString: Begin") << std::endl;

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

  for ( size_t i = 0; i < 10; ++i )
  {
    s1 += genString(i);
    std::cout << FTL_STR("s1 += genString(") << i << FTL_STR("): ") << s1 << std::endl;
  }

  for ( size_t i = 30; i--; )
  {
    s1.resize( i );
    std::cout << FTL_STR("s1.resize(") << i << FTL_STR("): ") << s1 << std::endl;
  }

  std::cout << FTL_STR("SmallString: End") << std::endl;
}

void dumpOSM( OSM const &osm )
{
  std::cout << FTL_STR("  DumpOSM: Begin") << std::endl;
  for ( OSM::CIT it = osm.begin(); it != osm.end(); ++it )
    std::cout << FTL_STR("    '") << it->key() << FTL_STR("' -> '") << it->value() << FTL_STR("'") << std::endl;
  std::cout << FTL_STR("  DumpOSM: End") << std::endl;
}

void testOrderedStringMap()
{
  std::cout << FTL_STR("OrderedStringMap: Begin") << std::endl;

  OSM osm;
  std::cout << FTL_STR("  At start") << std::endl;
  dumpOSM( osm );

  for ( size_t vl = 0; vl < 32; vl += 1 )
    for ( size_t kl = 0; kl < 32; kl += 1 )
      osm[genString(kl)] = genString(vl);
  for ( size_t kl = 0; kl < 32; kl += 2 )
    osm[genString(kl)] = genString(8);
  std::cout << FTL_STR("  After populate") << std::endl;
  dumpOSM( osm );

  for ( size_t kl = 0; kl < 40; kl += 1 )
  {
    OSM::KeyTy key = genString(kl);
    OSM::CIT it = osm.find( key );
    if ( it != osm.end() )
      std::cout << FTL_STR("  find('") << key << FTL_STR("') = '") << it->value() << FTL_STR("'") << std::endl;
    else
      std::cout << FTL_STR("  find('") << key << FTL_STR("') = none") << std::endl;
  }

  std::cout << FTL_STR("OrderedStringMap: End") << std::endl;
}

int main( int argc, char **argv )
{
  // sleep( 20 );
  // sleep( 5 );

  dumpAllocatedPtrs();
  testSmallString();
  dumpAllocatedPtrs();
  testOrderedStringMap();
  dumpAllocatedPtrs();
}
