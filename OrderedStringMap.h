/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_OrderedStringMap_h
#define _FTL_OrderedStringMap_h

#include <FTL/StrRef.h>
#include <map>
#include <string>
#if defined(FTL_PLATFORM_WINDOWS)
# include <unordered_map>
#else
# include <tr1/unordered_map>
#endif
#include <vector>

FTL_NAMESPACE_BEGIN

template<class ValueTy>
class OrderedStringMap
{
  OrderedStringMap( OrderedStringMap const & );
  OrderedStringMap &operator=( OrderedStringMap const & );

  typedef std::pair<std::string, ValueTy> KV;
  typedef std::vector<KV> Vec;
#if defined(FTL_PLATFORM_WINDOWS)
  typedef std::unordered_map<
#else
  typedef std::tr1::unordered_map<
#endif
    StrRef,
    size_t,
    StrRef::Hash,
    StrRef::Equals
    > Map;

public:

  OrderedStringMap() {}
  ~OrderedStringMap() {}

  bool empty() const
    { return m_vec.empty(); }

  bool size() const
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
    m_map.clear();
    m_vec.clear();
  }

  bool insertTakingKey( std::string &key, ValueTy const &value )
  {
    size_t index = m_vec.size();
    m_vec.resize( index + 1 );
    KV &kv = m_vec[index];
    kv.first.swap( key );
    kv.second = value;
    std::pair<Map::iterator, bool> insertResult = m_map.insert(
      std::pair<StrRef, size_t>( kv.first, index )
      );
    if ( !insertResult.second )
    {
      kv.first.swap( key );
      m_vec.resize( index );
    }
    return insertResult.second;
  }

  bool insert( StrRef key, ValueTy const &value )
  {
    std::string keyString = key;
    return insertTakingKey( keyString, value );
  }

private:

  Map m_map;
  Vec m_vec;
};

FTL_NAMESPACE_END

#endif //_FTL_OrderedStringMap_h
