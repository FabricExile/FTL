/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <assert.h>
#include <stddef.h>
#include <iterator>
#include <vector>

FTL_NAMESPACE_BEGIN

template<typename EleTy>
class ArrayRef
{
  EleTy const *_data;
  size_t _size;

public:

  typedef EleTy const *IT;
  typedef std::reverse_iterator<IT> RIT;

  IT begin() const { return _data; }
  IT end() const { return _data + _size; }

  RIT rbegin() const { return RIT( end() ); }
  RIT rend() const { return RIT( begin() ); }

  ArrayRef() : _data(0), _size(0) {}
  ArrayRef( EleTy const &ele )
    : _data( &ele )
    , _size(1) {}
  ArrayRef( EleTy const *data, size_t size )
    : _data( data )
    , _size( size )
  {
    assert( _size == 0 || !!_data );
  }
  ArrayRef( typename std::vector<EleTy> const &vec )
    : _data( vec.empty()? 0: vec.data() )
    , _size( vec.size() )
  {
    assert( _size == 0 || !!_data );
  }
  ArrayRef( IT b, IT e )
  {
    assert( b <= e );
    _data = b;
    _size = e - b;
  }
  ArrayRef( RIT b, RIT e )
  {
    assert( b.base() >= e.base() );
    _data = e.base();
    _size = b.base() - e.base();
  }

  size_t size() const { return _size; }
  bool empty() const { return _size == 0; }
  EleTy const *data() const { return _data; }

  EleTy const &operator[]( size_t index ) const
  {
    assert( index < _size );
    return _data[index];
  }

  EleTy const &front() const
  {
    assert( !empty() );
    return *begin();
  }
  ArrayRef drop_front( size_t count = 1 ) const
  {
    assert( size() >= count );
    return ArrayRef( begin() + count, end() );
  }

  EleTy const &back() const
  {
    assert( !empty() );
    return *rbegin();
  }
  ArrayRef drop_back( size_t count = 1 ) const
  {
    assert( size() >= count );
    return ArrayRef( begin(), end() - count );
  }

  ArrayRef head( size_t count ) const
    { return ArrayRef( begin(), begin() + std::min( size(), count ) ); }
};

FTL_NAMESPACE_END
