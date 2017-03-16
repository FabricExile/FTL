/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>
#include <FTL/CStrRef.h>
#include <FTL/SmallString.h>

#include <exception>
#include <stdint.h>
#include <string>
#include <vector>

FTL_NAMESPACE_BEGIN

class OrderStringMapException : public std::exception
{
public:

  OrderStringMapException() {}
};

class OrderStringMapFullException : public OrderStringMapException
{
public:

  OrderStringMapFullException() {}

  virtual char const *what() const throw()
    { return "OrderedStringMap is full"; }
};

template<
  typename ValueTy,
  unsigned MinBucketCountLog2 = 6,
  typename IndTy = uint32_t,
  unsigned SmallStringSize = 12
  >
class OrderedStringMap
{
public:

  typedef SmallString<SmallStringSize, IndTy> KeyTy;

private:

  OrderedStringMap( OrderedStringMap const & ) FTL_DELETED_FUNCTION;
  OrderedStringMap &operator=( OrderedStringMap const & ) FTL_DELETED_FUNCTION;

  struct Bucket
  {
    IndTy entryIndex;

    Bucket()
      { makeUnused(); }

    bool isUsed() const
      { return entryIndex != ~IndTy(0); }

    void makeUnused()
      { entryIndex = ~IndTy(0); }
  };
  typedef std::vector<Bucket> BucketVec;

  class Entry
  {
  public:

    Entry() {}

    Entry( Entry const &that )
      : m_key( that.m_key )
      , m_keyHash( that.m_keyHash )
      , m_value( that.m_value )
      {}

    Entry &operator=( Entry const &that )
    {
      m_key = that.m_key;
      m_keyHash = that.m_keyHash;
      m_value = that.m_value;
      return *this;
    }

#if FTL_HAS_RVALUE_REFERENCES
    Entry( Entry &&that )
      : m_key( std::move( that.m_key ) )
      , m_keyHash( that.m_keyHash )
      , m_value( std::move( that.m_value ) )
      {}

    Entry &operator=( Entry &&that )
    {
      m_key = std::move( that.m_key );
      m_keyHash = that.m_keyHash;
      m_value = std::move( that.m_value );
      return *this;
    }
#endif

    Entry(
      StrRef keyStr,
      IndTy keyHash,
      ValueTy const &value
      )
      : m_key( keyStr )
      , m_keyHash( keyHash )
      , m_value( value )
      {}

#if FTL_HAS_RVALUE_REFERENCES
    Entry(
      StrRef keyStr,
      IndTy keyHash,
      ValueTy &&value
      )
      : m_key( keyStr )
      , m_keyHash( keyHash )
      , m_value( std::move( value ) )
      {}

    Entry(
      SmallString<16> &&key,
      IndTy keyHash,
      ValueTy const &value
      )
      : m_key( std::move( key ) )
      , m_keyHash( keyHash )
      , m_value( value )
      {}

    Entry(
      SmallString<16> &&key,
      IndTy keyHash,
      ValueTy &&value
      )
      : m_key( std::move( key ) )
      , m_keyHash( keyHash )
      , m_value( std::move( value ) )
      {}
#endif

    IndTy keyHash() const
      { return m_keyHash; }

    CStrRef key() const
      { return m_key; }

    ValueTy const &value() const
      { return m_value; }

    ValueTy &value()
      { return m_value; }

  private:

    KeyTy m_key;
    IndTy m_keyHash;
    ValueTy m_value;
  };

  typedef std::vector<Entry> EntryVec;

public:

  OrderedStringMap() {}
  ~OrderedStringMap() {}

  bool empty() const
    { return m_entries.empty(); }

  IndTy size() const
    { return m_entries.size(); }

  ValueTy const &operator[]( IndTy index ) const
    { return m_entries[index].value(); }

  ValueTy &operator[]( IndTy index )
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

  IndTy count( StrRef key ) const
  {
    const_iterator it = find( key );
    if ( it != m_entries.end() )
      return 1;
    else
      return 0;
  }

  bool has( StrRef key ) const
    { return count( key ) > 0; }

  void clear()
  {
    if ( !m_buckets.empty() )
    {
      m_buckets.resize( IndTy(1) << MinBucketCountLog2 );
      for ( typename BucketVec::iterator it = m_buckets.begin();
        it != m_buckets.end(); ++it )
        it->makeUnused();
    }
    m_entries.clear();
  }

  bool insert( StrRef key, ValueTy const &value )
  {
    prepareForInsert();

    const IndTy keyHash = key.hash();
    Bucket &bucket = findBucket( key, keyHash );
    if ( bucket.isUsed() )
      return false;

    IndTy entryIndex = m_entries.size();
    m_entries.push_back( Entry( key, keyHash, value ) );

    bucket.entryIndex = entryIndex;

    return true;
  }

#if FTL_HAS_RVALUE_REFERENCES
  bool insert( KeyTy &&key, ValueTy const &value )
  {
    prepareForInsert();

    const IndTy keyHash = key.hash();
    Bucket &bucket = findBucket( key, keyHash );
    if ( bucket.isUsed() )
      return false;

    IndTy entryIndex = m_entries.size();
    m_entries.push_back( Entry( std::move( key ), keyHash, value ) );

    bucket.entryIndex = entryIndex;

    return true;
  }

  bool insert( StrRef key, ValueTy &&value )
  {
    prepareForInsert();

    const IndTy keyHash = key.hash();
    Bucket &bucket = findBucket( key, keyHash );
    if ( bucket.isUsed() )
      return false;

    IndTy entryIndex = m_entries.size();
    m_entries.push_back( Entry( key, keyHash, std::move( value ) ) );

    bucket.entryIndex = entryIndex;

    return true;
  }

  bool insert( KeyTy &&key, ValueTy &&value )
  {
    prepareForInsert();

    const IndTy keyHash = key.hash();
    Bucket &bucket = findBucket( key, keyHash );
    if ( bucket.isUsed() )
      return false;

    IndTy entryIndex = m_entries.size();
    m_entries.push_back(
      Entry( std::move( key ), keyHash, std::move( value ) )
      );

    bucket.entryIndex = entryIndex;

    return true;
  }
#endif

private:

  void prepareForInsert()
  {
    if ( m_entries.size() == (~IndTy(0) / 2) )
      throw OrderStringMapFullException();

    if ( m_entries.size() + 1 > m_buckets.size() / 2 )
    {
      rehash(
        std::max(
          2 * IndTy( m_buckets.size() ),
          IndTy(1) << MinBucketCountLog2
          )
        );

      m_entries.reserve( 2 * m_entries.size() );
    }
  }

  // Always returns a hash bucket index.  If the resulting bucket.isUsed(),
  // the key matches; otherwise, the entry is empty.
  IndTy findBucketIndex(
    StrRef key,
    IndTy keyHash
    ) const
  {
    const IndTy bucketsSize = m_buckets.size();
    const IndTy bucketsMask = bucketsSize - 1;

    IndTy bucketIndex = keyHash & bucketsMask;
    IndTy i = 0;
    for (;;)
    {
      Bucket const &bucket = m_buckets[bucketIndex];
      if ( !bucket.isUsed() )
        return bucketIndex;
      Entry const &entry = m_entries[bucket.entryIndex];
      if ( entry.keyHash() == keyHash
        && entry.key() == key )
        return bucketIndex;

      bucketIndex = ( bucketIndex + (++i) ) & bucketsMask;
    }
  }

  Bucket const &findBucket(
    StrRef key,
    IndTy keyHash
    ) const
  {
    IndTy bucketIndex = findBucketIndex( key, keyHash );
    assert( bucketIndex < m_buckets.size() );
    return m_buckets[bucketIndex];
  }

  Bucket &findBucket(
    StrRef key,
    IndTy keyHash
    )
  {
    IndTy bucketIndex = findBucketIndex( key, keyHash );
    assert( bucketIndex < m_buckets.size() );
    return m_buckets[bucketIndex];
  }

  void rehash( IndTy bucketsSize )
  {
    if ( IndTy( m_buckets.size() ) != bucketsSize )
    {
      IndTy bucketsMask = bucketsSize - 1;
      assert( ( bucketsSize & bucketsMask ) == 0 );

      for ( typename BucketVec::iterator it = m_buckets.begin();
        it != m_buckets.end(); ++it )
        it->makeUnused();
      m_buckets.resize( bucketsSize );

      for ( typename EntryVec::const_iterator it = m_entries.begin();
        it != m_entries.end(); ++it )
      {
        Entry const &entry = *it;

        IndTy bucketIndex = entry.keyHash() & bucketsMask;
        IndTy i = 0;
        for (;;)
        {
          Bucket &bucket = m_buckets[bucketIndex];
          if ( !bucket.isUsed() )
          {
            bucket.entryIndex = it - m_entries.begin();
            break;
          }
          bucketIndex = ( bucketIndex + (++i) ) & bucketsMask;
        }
      }
    }
  }

  BucketVec m_buckets;
  EntryVec m_entries;
};

FTL_NAMESPACE_END
