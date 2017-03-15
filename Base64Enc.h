/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>

#include <string>

FTL_NAMESPACE_BEGIN

template<typename StringTy>
class Base64Enc
{
public:

  static size_t RequiredSize(
    size_t size
    )
  {
    return 4 * ( ( size + 2 ) / 3 );
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
  
  Base64Enc( StringTy &string )
    : m_string( string )
    , m_offset( 0 )
  {
  }

  ~Base64Enc()
  {
    if ( m_offset != 0 )
      outputBlock( m_offset + 1 );
  }

  void append( void const *dataVoidPtr, size_t size )
  {
    uint8_t const *data = reinterpret_cast<uint8_t const *>( dataVoidPtr );
    uint8_t const * const dataEnd = data + size;
    while ( data != dataEnd )
    {
      uint8_t byte = *data++;

      switch ( m_offset++ )
      {
        case 0:
          m_inds[0]  = ( byte & UINT8_C(0xFC) ) >> 2; // XXXXXX.. -> XXXXXX
          m_inds[1]  = ( byte & UINT8_C(0x03) ) << 4; // ......XX -> XX....
          break;

        case 1:
          m_inds[1] |= ( byte & UINT8_C(0xF0) ) >> 4; // XXXX.... -> ..XXXX
          m_inds[2]  = ( byte & UINT8_C(0x0F) ) << 2; // ....XXXX -> XXXX..
          break;

        case 2:
          m_inds[2] |= ( byte & UINT8_C(0xC0) ) >> 6; // XX...... -> ....XX
          m_inds[3]  = ( byte & UINT8_C(0x3F) ) >> 0; // ..XXXXXX -> XXXXXX
          break;
      }
      if ( m_offset == 3 )
      {
        outputBlock( 4 );
        m_offset = 0;
      }
    }
  }

  template<typename EleTy>
  Base64Enc &operator<<( ArrayRef<EleTy> blob )
  {
    append( blob.data(), blob.dataSize() );
    return *this;
  }

  template<typename EleTy>
  Base64Enc &operator<<( ArrayRef< ArrayRef<EleTy> > blobs )
  {
    for ( typename ArrayRef< ArrayRef<EleTy> >::IT it = blobs.begin();
      it != blobs.end(); ++it )
      append( it->data(), it->dataSize() );
    return *this;
  }

  Base64Enc &operator<<( StrRef str )
  {
    append( str.data(), str.size() );
    return *this;
  }

  Base64Enc &operator<<( ArrayRef<StrRef> strs )
  {
    for ( ArrayRef<StrRef>::IT it = strs.begin();
      it != strs.end(); ++it )
      append( it->data(), it->size() );
    return *this;
  }

private:

  void outputBlock( uint8_t indCount ) const
  {
    assert( indCount > 0 && indCount <= 4 );

    for ( uint8_t i = 0; i < 4; ++i )
    {
      if ( i < indCount )
      {
        static const char Base64Chars[] =
          "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        m_string += Base64Chars[m_inds[i]];
      }
      else m_string += '=';
    }
  }

  StringTy &m_string;
  uint8_t m_inds[4];
  uint8_t m_offset;
};

FTL_NAMESPACE_END
