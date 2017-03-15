/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/CStrRef.h>
#include <map>
#include <string>
#include <vector>

FTL_NAMESPACE_BEGIN

template<class ValueTy, size_t MinHashesSize = 64>
class OrderedStringMap
{
  OrderedStringMap( OrderedStringMap const & ) FTL_DELETED_FUNCTION;
  OrderedStringMap &operator=( OrderedStringMap const & ) FTL_DELETED_FUNCTION;

  struct Hash
  {
    size_t keyIndex;
    size_t keySize;
    size_t keyHash;
    size_t valueIndex;

    Hash()
      { makeUnused(); }

    bool isUsed() const
      { return keyIndex != ~size_t(0); }

    void makeUnused()
      { keyIndex = ~size_t(0); }
  };

public:

  OrderedStringMap()
    : m_usedEntries( 0 )
    {}
  ~OrderedStringMap() { clear(); }

  bool empty() const
    { return m_vec.empty(); }

  size_t size() const
    { return m_vec.size(); }

  bool has( StrRef key ) const
    { return m_map.count( key ) > 0; }

  typedef typename Vec::const_iterator const_iterator;

  const_iterator begin() const
    { return m_vec.begin(); }

  const_iterator end() const
    { return m_vec.end(); }

  const_iterator find( StrRef key ) const
  {
    Map::const_iterator it = m_map.find( key );
    if ( it != m_map.end() )
      return begin() + it->second;
    else
      return end();
  }

  void clear()
  {
    m_keys.clear();
    m_hashes.resize( MinHashesSize );
    for ( std::vector<Hash>::iterator it = m_hashes.begin();
      it != m_hashes.end(); ++it )
      it->makeUnused();
    m_vec.clear();
  }

  bool insert( StrRef key, ValueTy const &value )
  {
    if ( m_usedEntries + 1 >= m_hashes.size() / 2 )
    {
      std::vector<Hash> newHashes;
      newHashes.resize( std::min( 2 * m_hashes.size(), MinHashesSize ) );
      rehash( newHashes );
    }

    size_t keyHash = key.hash();
    size_t hashesSize = m_hashes.size();
    size_t hashesMask = hashesSize - 1;

    size_t h = keyHash & hashesMask;
    for ( size_t i = 1; ; ++i )
    {
      Hash const &thatHash = newHashes[h];
      if ( !thatHash.isUsed() )
        break;

      if ( StrRef( &m_keys[thatHash.keyIndex], thatHash.keySize ) == key )
        return false;

      h = ( h + i ) & hashesMask;
    }

    ++m_usedEntries;

    size_t valueIndex = m_values.size();
    m_values.push_back( value );

    size_t keyIndex = m_keys.size();
    m_keys.resize( keyIndex + key.size() );
    memcpy( m_keys.data() + keyIndex, key.data(), key.size() );

    Hash &hash = m_hashes[h];
    assert( !hash.isUsed() );
    hash.keyHash = keyHash;
    hash.keyIndex = keyIndex;
    hash.keySize = key.size();
    hash.valueIndex = valueIndex;
  }

private:

  void rehash( std::vector<Hash> &newHashes )
  {
    size_t newHashesSize = newHashes.size();
    size_t newHashesMask = newHashesSize - 1;

    for ( std::vector<Hash>::const_iterator it = m_hashes.begin();
      it != m_hashes.end(); ++it )
    {
      Hash const &hash = *it;
      if ( !hash.isUsed() )
        continue;

      size_t h = hash.keyHash & newHashesMask;
      for ( size_t i = 1; ; ++i )
      {
        Hash const &thatHash = newHashes[h];
        if ( !thatHash.isUsed() )
          break;
        h = ( h + i ) & hashesMask;
      }
      newHashes[h] = hash;
    }

    m_hashes.swap( newHashes );
  }

  std::vector<char> m_keys;
  std::vector<Hash> m_hashes;
  std::vector<ValueTy> m_values;
  size_t m_usedEntries;
};

FTL_NAMESPACE_END
