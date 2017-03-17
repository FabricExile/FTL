/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/CStrRef.h>

#include <string>

FTL_NAMESPACE_BEGIN

template<
  unsigned SmallSize,
  typename IndTy = size_t
  >
class SmallString
{
public:

  SmallString()
    : m_size( 0 )
    {}

  SmallString( SmallString const &that )
    : m_size( that.m_size )
  {
    if ( m_size >= SmallSize )
    {
      m_large.m_alloc = m_size;
      m_large.m_data = new char[m_size + 1];
    }
    memcpy( mutableData(), that.data(), m_size );
  }

  SmallString &operator=( SmallString const &that )
  {
    IndTy oldSize = m_size;
    m_size = that.m_size;
    if ( oldSize < SmallSize )
    {
      if ( m_size < SmallSize )
        ; // nothing
      else
      {
        m_large.m_alloc = m_size;
        m_large.m_data = new char[m_size + 1];
      }
    }
    else // oldSize >= SmallSize
    {
      if ( m_size < SmallSize )
        delete [] m_large.m_data;
      else if ( m_large.m_alloc < m_size )
      {
        delete [] m_large.m_data;
        m_large.m_alloc = m_size;
        m_large.m_data = new char[m_size + 1];
      }
    }

    memcpy( mutableData(), that.data(), m_size );

    return *this;
  }

#if FTL_HAS_RVALUE_REFERENCES
  SmallString( SmallString &&that )
    : m_size( that.m_size )
  {
    if ( m_size < SmallSize )
      memcpy( m_small.m_data, that.m_small.m_data, m_size );
    else
    {
      m_large.m_alloc = that.m_large.m_alloc;
      m_large.m_data = that.m_large.m_data;
      that.m_size = 0;
    }
  }

  SmallString &operator=( SmallString &&that )
  {
    if ( m_size >= SmallSize )
      delete [] m_large.m_data;

    m_size = that.m_size;
    if ( m_size < SmallSize )
      memcpy( m_small.m_data, that.m_small.m_data, m_size );
    else
    {
      m_large.m_alloc = that.m_large.m_alloc;
      m_large.m_data = that.m_large.m_data;
      that.m_size = 0;
    }
  }
#endif

  SmallString( StrRef str )
    : m_size( 0 )
    { append( str ); }

  SmallString &operator=( StrRef str )
  {
    resize( str.size() );
    memcpy( mutableData(), str.data(), str.size() );
    return *this;
  }

  ~SmallString()
  {
    if ( m_size >= SmallSize )
      delete [] m_large.m_data;
  }

  IndTy size() const
    { return m_size; }

  bool empty() const
    { return m_size == 0; }

  char const *data() const
    { return mutableData(); }

  StrRef str() const
    { return StrRef( data(), size() ); }

  operator StrRef() const
    { return str(); }

  char const *c_str() const
  {
    char *data = mutableData();
    data[m_size] = 0;
    return data;
  }

  CStrRef cstr() const
    { return CStrRef( c_str(), size() ); }

  operator CStrRef() const
    { return cstr(); }

  char operator[]( IndTy index ) const
  {
    assert( index < size() );
    return mutableData()[index];
  }

  char &operator[]( IndTy index )
  {
    assert( index < size() );
    return mutableData()[index];
  }

  void clear()
  {
    if ( m_size >= SmallSize )
      delete [] m_large.m_data;
    m_size = 0;
  }

  void resize( IndTy newSize )
  {
    IndTy oldSize = m_size;
    if ( newSize < SmallSize )
    {
      if ( oldSize >= SmallSize )
      {
        char *data = m_large.m_data;
        memcpy( m_small.m_data, data, newSize );
        delete [] data;
      }
    }
    else if ( oldSize < SmallSize )
    {
      if ( newSize >= SmallSize )
      {
        IndTy alloc = ( newSize * 3 + 1 ) / 2;
        char *data = new char[alloc + 1];
        memcpy( data, m_small.m_data, oldSize );
        m_large.m_alloc = alloc;
        m_large.m_data = data;
      }
    }
    else if ( m_large.m_alloc < newSize )
    {
      m_large.m_alloc = ( newSize * 3 + 1 ) / 2;
      char *oldData = m_large.m_data;
      m_large.m_data = new char[m_large.m_alloc + 1];
      memcpy( m_large.m_data, oldData, oldSize );
      delete [] oldData;
    }
    m_size = newSize;
  }

  void push_back( char ch )
  {
    IndTy oldSize = size();
    resize( oldSize + 1 );
    mutableData()[oldSize] = ch;
  }

  SmallString &operator+=( char ch )
  {
    push_back( ch );
    return *this;
  }

  void append( StrRef str )
  {
    IndTy strSize = str.size();
    if ( strSize > 0 )
    {
      IndTy oldSize = size();
      resize( oldSize + strSize );
      memcpy( mutableData() + oldSize, str.data(), strSize );
    }
  }

  SmallString &operator+=( StrRef str )
  {
    append( str );
    return *this;
  }

  void swap( SmallString &that )
  {
    if ( m_size < SmallSize )
    {
      if ( that.m_size < SmallSize )
      {
        IndTy maxSize = std::max( m_size, that.m_size );
        for ( IndTy i = 0; i < maxSize; ++i )
        {
          if ( i < m_size )
          {
            if ( i < that.m_size )
              std::swap( m_small.m_data[i], that.m_small.m_data[i] );
            else
              that.m_small.m_data[i] = m_small.m_data[i];
          }
          else
          {
            if ( i < that.m_size )
              m_small.m_data[i] = that.m_small.m_data[i];
          }
        }
      }
      else
      {
        IndTy thatAlloc = that.m_large.m_alloc;
        char *thatData = that.m_large.m_data;
        memcpy( that.m_small.m_data, m_small.m_data, m_size );
        m_large.m_alloc = thatAlloc;
        m_large.m_data = thatData;
      }
    }
    else
    {
      if ( that.m_size < SmallSize )
      {
        IndTy alloc = m_large.m_alloc;
        char *data = m_large.m_data;
        memcpy( m_small.m_data, that.m_small.m_data, that.m_size );
        that.m_large.m_alloc = alloc;
        that.m_large.m_data = data;
      }
      else
      {
        std::swap( m_large.m_alloc, that.m_large.m_alloc );
        std::swap( m_large.m_data, that.m_large.m_data );
      }
    }
    std::swap( m_size, that.m_size );
  }

  size_t hash() const
    { return str().hash(); }

private:

  char *mutableData() const
  {
    if ( m_size < SmallSize )
      return m_small.m_data;
    else
      return m_large.m_data;
  }

  IndTy m_size;
  union
  {
    mutable struct
    {
      char m_data[SmallSize];
    } m_small;
    mutable struct
    {
      IndTy m_alloc;
      char *m_data;
    } m_large;
  };
};

FTL_NAMESPACE_END

template<
  unsigned SmallSize,
  typename IndTy
  >
std::ostream &operator<<(
  std::ostream &os,
  FTL::SmallString<SmallSize, IndTy> const &ss
  )
{
  return operator<<( os, ss.str() );
}
