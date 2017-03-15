/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/CStrRef.h>

#include <string>
#include <vector>

FTL_NAMESPACE_BEGIN

template<
  class ValueTy,
  unsigned MinBucketCountLog2 = 6
  >
class OrderedStringMap
{
  OrderedStringMap( OrderedStringMap const & ) FTL_DELETED_FUNCTION;
  OrderedStringMap &operator=( OrderedStringMap const & ) FTL_DELETED_FUNCTION;

  struct Bucket
  {
    size_t keyIndex;
    size_t keySize;
    size_t keyHash;
    size_t entryIndex;

    Bucket()
      { makeUnused(); }

    bool isUsed() const
      { return keyIndex != ~size_t(0); }

    void makeUnused()
      { keyIndex = ~size_t(0); }
  };
  typedef std::vector<Bucket> BucketVec;

public:

  class Entry
  {
  public:

    Entry() : m_keyIndex( ~size_t(0) ) {}

    Entry( Entry const &that )
      : m_keyIndex( that.m_keyIndex )
      , m_keySize( that.m_keySize )
      , m_value( that.m_value )
      {}

    Entry &operator=( Entry const &that )
    {
      m_keyIndex = that.m_keyIndex;
      m_keySize = that.m_keySize;
      m_value = that.m_value;
      return *this;
    }

#if FTL_HAS_RVALUE_REFERENCES
    Entry( Entry &&that )
      : m_keyIndex( that.m_keyIndex )
      , m_keySize( that.m_keySize )
      , m_value( std::move( that.m_value ) )
      {}

    Entry &operator=( Entry &&that )
    {
      m_keyIndex = that.m_keyIndex;
      m_keySize = that.m_keySize;
      m_value = std::move( that.m_value );
      return *this;
    }
#endif

    // [pz 20170315] Do not use outside of this class; will become private
    Entry(
      size_t keyIndex,
      size_t keySize,
      ValueTy const &value
      )
      : m_keyIndex( keyIndex )
      , m_keySize( keySize )
      , m_value( value )
      {}

#if FTL_HAS_RVALUE_REFERENCES
    // [pz 20170315] Do not use outside of this class; will become private
    Entry(
      size_t keyIndex,
      size_t keySize,
      ValueTy &&value
      )
      : m_keyIndex( keyIndex )
      , m_keySize( keySize )
      , m_value( std::move( value ) )
      {}
#endif

    CStrRef key( OrderedStringMap const &map ) const
      { return CStrRef( map.m_keys.data() + m_keyIndex, m_keySize ); }

    ValueTy const &value() const
      { return m_value; }

    ValueTy &value()
      { return m_value; }

    size_t m_keyIndex;
    size_t m_keySize;
    ValueTy m_value;
  };

  typedef std::vector<Entry> EntryVec;

  OrderedStringMap() {}
  ~OrderedStringMap() {}

  bool empty() const
    { return m_entries.empty(); }

  size_t size() const
    { return m_entries.size(); }

  ValueTy const &operator[]( size_t index ) const
    { return m_entries[index].value(); }

  ValueTy &operator[]( size_t index )
    { return m_entries[index].value(); }

  typedef typename EntryVec::iterator iterator;

  iterator begin()
    { return m_entries.begin(); }

  iterator end()
    { return m_entries.end(); }

  iterator find( StrRef key )
  {
    Bucket const &bucket = findBucket( key, key.hash() );
    if ( bucket.isUsed() )
      return m_entries.begin() + bucket.entryIndex;
    else
      return m_entries.end();
  }

  typedef typename EntryVec::const_iterator const_iterator;

  const_iterator begin() const
    { return m_entries.begin(); }

  const_iterator end() const
    { return m_entries.end(); }

  const_iterator find( StrRef key ) const
  {
    Bucket const &bucket = findBucket( key, key.hash() );
    if ( bucket.isUsed() )
      return m_entries.begin() + bucket.entryIndex;
    else
      return m_entries.end();
  }

  size_t count( StrRef key ) const
  {
    const_iterator it = find( key );
    if ( it != begin() )
      return 1;
    else
      return 0;
  }

  bool has( StrRef key ) const
    { return count( key ) > 0; }

  void clear()
  {
    m_keys.clear();
    if ( !m_buckets.empty() )
    {
      m_buckets.resize( size_t(1) << MinBucketCountLog2 );
      for ( typename BucketVec::iterator it = m_buckets.begin();
        it != m_buckets.end(); ++it )
        it->makeUnused();
    }
    m_entries.clear();
  }

  bool insert( StrRef key, ValueTy const &value )
  {
    if ( m_entries.size() + 1 >= m_buckets.size() / 2 )
    {
      BucketVec newBuckets;
      newBuckets.resize(
        std::max( 2 * m_buckets.size(), size_t(1) << MinBucketCountLog2 )
        );
      if ( !m_entries.empty() )
        rehashTo( newBuckets );
      m_buckets.swap( newBuckets );
    }

    const size_t keyHash = key.hash();
    Bucket &bucket = findBucket( key, keyHash );
    if ( bucket.isUsed() )
      return false;

    size_t keyIndex = m_keys.size();
    size_t keySize = key.size();
    m_keys.resize( keyIndex + keySize + 1 );
    char *keyData = m_keys.data() + keyIndex;
    memcpy( keyData, key.data(), keySize );
    keyData[keySize] = 0;

    size_t entryIndex = m_entries.size();
    m_entries.push_back( Entry( keyIndex, keySize, value ) );

    bucket.keyHash = keyHash;
    bucket.keyIndex = keyIndex;
    bucket.keySize = keySize;
    bucket.entryIndex = entryIndex;

    return true;
  }

private:

  // Always returns a hash bucket index.  If the resulting bucket.isUsed(),
  // the key matches; otherwise, the entry is empty.
  size_t findBucketIndex(
    StrRef key,
    size_t keyHash
    ) const
  {
    const size_t bucketsSize = m_buckets.size();
    const size_t bucketsMask = bucketsSize - 1;

    size_t bucketIndex = keyHash & bucketsMask;
    size_t i = 0;
    for (;;)
    {
      Bucket const &bucket = m_buckets[bucketIndex];
      if ( !bucket.isUsed()
        || StrRef( m_keys.data() + bucket.keyIndex, bucket.keySize ) == key )
        return bucketIndex;

      bucketIndex = ( bucketIndex + (++i) ) & bucketsMask;
    }
  }

  Bucket const &findBucket(
    StrRef key,
    size_t keyHash
    ) const
  {
    size_t bucketIndex = findBucketIndex( key, keyHash );
    assert( bucketIndex < m_buckets.size() );
    return m_buckets[bucketIndex];
  }

  Bucket &findBucket(
    StrRef key,
    size_t keyHash
    )
  {
    size_t bucketIndex = findBucketIndex( key, keyHash );
    assert( bucketIndex < m_buckets.size() );
    return m_buckets[bucketIndex];
  }

  void rehashTo( std::vector<Bucket> &newBuckets )
  {
    size_t newBucketsSize = newBuckets.size();
    size_t newBucketsMask = newBucketsSize - 1;

    for ( typename BucketVec::const_iterator it = m_buckets.begin();
      it != m_buckets.end(); ++it )
    {
      Bucket const &oldBucket = *it;
      if ( !oldBucket.isUsed() )
        continue;

      size_t newBucketIndex = oldBucket.keyHash & newBucketsMask;
      size_t i = 0;
      for (;;)
      {
        Bucket const &newBucket = newBuckets[newBucketIndex];
        if ( !newBucket.isUsed() )
          break;
        newBucketIndex = ( newBucketIndex + (++i) ) & newBucketsMask;
      }
      newBuckets[newBucketIndex] = oldBucket;
    }
  }

  std::vector<char> m_keys;
  BucketVec m_buckets;
  EntryVec m_entries;
};

FTL_NAMESPACE_END
