/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>

#include <string>

FTL_NAMESPACE_BEGIN

template<typename ArrayTy>
class Base64Dec
{
public:

  static size_t RequiredSize(
    size_t size
    )
  {
    return 3 * ( ( size + 3 ) / 4 );
  }

  template<typename EleTy>
  static size_t RequiredSize(
    ArrayRef<EleTy> blob
    )
  {
    return RequiredSize( blob.dataSize() );
  }
  
  template<typename EleTy>
  static size_t RequiredSize(
    ArrayRef< ArrayRef<EleTy> > blobs
    )
  {
    size_t totalDataSize = 0;
    for ( typename ArrayRef< ArrayRef<EleTy> >::IT it = blobs.begin();
      it != blobs.end(); ++it )
      totalDataSize += it->dataSize();
    return RequiredSize( totalDataSize );
  }

  static size_t RequiredSize(
    StrRef str
    )
  {
    return RequiredSize( str.size() );
  }
  
  static size_t RequiredSize(
    ArrayRef<StrRef> strs
    )
  {
    size_t totalSize = 0;
    for ( ArrayRef<StrRef>::IT it = strs.begin(); it != strs.end(); ++it )
      totalSize += it->size();
    return RequiredSize( totalSize );
  }
  
  Base64Dec( ArrayTy &array )
    : m_array( array )
    , m_offset( 0 )
  {
  }

  ~Base64Dec()
  {
    if ( m_offset > 0 )
      outputBlock( m_offset );
  }

  void append( StrRef str )
  {
    while ( !str.empty() )
    {
      static const uint8_t Base64Inds[256] =
      {
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255,  62, 255, 255, 255,  63,
         52,  53,  54,  55,  56,  57,  58,  59,
         60,  61, 255, 255, 255, 255, 255, 255,
        255,   0,   1,   2,   3,   4,   5,   6,
          7,   8,   9,  10,  11,  12,  13,  14,
         15,  16,  17,  18,  19,  20,  21,  22,
         23,  24,  25, 255, 255, 255, 255, 255,
        255,  26,  27,  28,  29,  30,  31,  32,
         33,  34,  35,  36,  37,  38,  39,  40,
         41,  42,  43,  44,  45,  46,  47,  48,
         49,  50,  51, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255
      };
      char ch = str.front();
      uint8_t ind = Base64Inds[uint8_t(ch)];
      str = str.drop_front();
      if ( ind < 64 )
      {
        m_inds[m_offset++] = ind;
        if ( m_offset == 4 )
        {
          outputBlock( 4 );
          m_offset = 0;
        }
      }
    }
  }

  Base64Dec &operator<<( StrRef str )
  {
    append( str );
    return *this;
  }

  Base64Dec &operator<<( ArrayRef<StrRef> strs )
  {
    for ( ArrayRef<StrRef>::IT it = strs.begin();
      it != strs.end(); ++it )
      append( *it );
    return *this;
  }

private:

  void outputBlock( uint8_t indCount ) const
  {
    assert( indCount > 0 && indCount <= 4 );

    uint8_t b0 = m_inds[0];
    if ( indCount > 1 )
    {
      m_array.push_back( (m_inds[0]<<2) | (m_inds[1]>>4) );

      if ( indCount > 2 )
      {
        m_array.push_back( (m_inds[1]<<4) | (m_inds[2]>>2) );

        if ( indCount > 3 )
          m_array.push_back( (m_inds[2]<<6) | (m_inds[3]>>0) );
      }
    }
  }

  ArrayTy &m_array;
  uint8_t m_inds[4];
  uint8_t m_offset;
};


FTL_NAMESPACE_END
