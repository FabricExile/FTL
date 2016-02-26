/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/CStrRef.h>
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

  typedef std::pair<CStrRef, ValueTy> KV;
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
    m_map.clear();
    for ( typename Vec::iterator it = m_vec.begin(); it != m_vec.end(); ++it )
      delete [] it->first.c_str();
    m_vec.clear();
  }

  bool insert( StrRef key, ValueTy const &value )
  {
    char *keyCStr = new char[key.size()+1];
    memcpy( keyCStr, key.data(), key.size() );
    keyCStr[key.size()] = '\0';

    size_t index = m_vec.size();
    m_vec.resize( index + 1 );
    KV &kv = m_vec[index];
    kv.first = CStrRef( keyCStr, key.size() );
    kv.second = value;
    std::pair<Map::iterator, bool> insertResult = m_map.insert(
      std::pair<StrRef, size_t>( kv.first, index )
      );
    if ( !insertResult.second )
    {
      delete [] keyCStr;
      m_vec.resize( index );
    }
    return insertResult.second;
  }

private:

  Map m_map;
  Vec m_vec;
};

FTL_NAMESPACE_END
