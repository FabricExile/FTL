/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Ticks.h>
#include <vector>
#include <map>

FTL_NAMESPACE_BEGIN

class ProfilingInfo
{
public:

  ProfilingInfo(char const * label, int parentIndex)
    : m_label(label)
    , m_parentIndex(parentIndex)
    , m_begin( GetCurrentTicks() )
    , m_elapsed( 0.0 )
  {
    m_end = m_begin;
  }

  ProfilingInfo()
    : m_label(NULL)
    , m_parentIndex(-1)
    , m_begin(0)
    , m_end(0)
    , m_elapsed(0.0)
  {
  }

  ProfilingInfo(const ProfilingInfo & other)
    : m_label(other.m_label)
    , m_parentIndex(other.m_parentIndex)
    , m_begin(other.m_begin)
    , m_end(other.m_end)
    , m_elapsed(other.m_elapsed)
  {
  }

  double getSeconds() const
  {
    return m_elapsed + GetSecondsBetweenTicks(m_begin, m_end);
  }

  double getMS() const
  {
    return getSeconds() * 1000.0;
  }

  char const *m_label;
  int m_parentIndex;
  uint64_t m_begin;
  uint64_t m_end;
  double m_elapsed;
};

struct ProfilingEntry
{
  ProfilingEntry(char const * label)
  : m_label(label)
  , m_level(0)
  , m_invocations(0)
  , m_seconds(0.0)
  {
  }

  char const * m_label;
  unsigned int m_level;
  unsigned int m_invocations;
  double m_seconds;
};

class ProfilingStack
{
public:

  ProfilingStack(bool enabled = true, unsigned int initialStackSize = 4096)
    : m_enabled(enabled)
    , m_numProfiles(0)
    , m_stackEnd(0)
  {
    m_profilingInfos.resize(m_enabled ? initialStackSize : 0);
    m_parentKeyStack.resize(m_enabled ? initialStackSize : 0);
  }

  int beginProfilingEvent(char const * label)
  {
    if(!m_enabled)
      return -1;

    int parentIndex = -1;
    if( m_stackEnd > 0 )
    {
      parentIndex = m_parentKeyStack[m_stackEnd-1];
    }

    if(m_numProfiles == m_profilingInfos.size())
      m_profilingInfos.resize(m_profilingInfos.size() * 2);
    if(m_stackEnd == m_parentKeyStack.size())
      m_parentKeyStack.resize(m_parentKeyStack.size() * 2);

    int index = m_numProfiles;
    m_profilingInfos[index] = ProfilingInfo(label, parentIndex);
    m_numProfiles++;
    m_parentKeyStack[m_stackEnd++] = index;

    return index;
  }

  bool endProfilingEvent(int index)
  {
    uint64_t endTick = GetCurrentTicks();
    if( index < 0 )
      return false;
    if( index >= m_numProfiles )
      return false;

    if( m_stackEnd == 0 )
      return false;

    m_profilingInfos[index].m_end = endTick;
    m_stackEnd--;

    return true;
  }

  bool stopProfilingEvent(int index)
  {
    uint64_t endTick = GetCurrentTicks();
    if( index < 0 )
      return false;
    if( index >= m_numProfiles )
      return false;

    m_profilingInfos[index].m_elapsed += GetSecondsBetweenTicks(m_profilingInfos[index].m_begin, endTick);
    m_profilingInfos[index].m_begin = m_profilingInfos[index].m_end = endTick;
    return true;
  }

  bool resumeProfilingEvent(int index)
  {
    uint64_t beginTick = GetCurrentTicks();
    if( index < 0 )
      return false;
    if( index >= m_numProfiles )
      return false;
    m_profilingInfos[index].m_begin = m_profilingInfos[index].m_end = beginTick;
    return true;
  }

  std::vector< ProfilingEntry > getReportEntries() const
  {
    std::vector< ProfilingEntry > entries;
    std::map< std::string, unsigned int > lookup;

    for(size_t i=0;i<m_numProfiles;i++)
    {
      std::string key = m_profilingInfos[i].m_label;
      int index = m_profilingInfos[i].m_parentIndex;
      while(index >= 0)
      {
        key += m_profilingInfos[index].m_label;
        index = m_profilingInfos[index].m_parentIndex;
      }
      
      std::map< std::string, unsigned int >::const_iterator it = lookup.find(key.c_str());
      if(it == lookup.end())
      {
        ProfilingEntry entry(m_profilingInfos[i].m_label);
        entry.m_invocations = 1;
        entry.m_level = 0;
        int index = m_profilingInfos[i].m_parentIndex;
        while(index >= 0)
        {
          entry.m_level++;
          index = m_profilingInfos[index].m_parentIndex;
        }

        entry.m_seconds = m_profilingInfos[i].getSeconds();
        lookup.insert(std::pair< std::string, unsigned int >(key.c_str(), (unsigned int)entries.size()));
        entries.push_back(entry);
      }
      else
      {
        entries[it->second].m_seconds += m_profilingInfos[i].getSeconds();
        entries[it->second].m_invocations++;
      }
    }

    return entries;
  }

  std::string getReportString() const
  {
    std::string result;
    std::vector< ProfilingEntry > entries = getReportEntries();
    std::string indent;
    for(size_t i=0;i<entries.size();i++)
    {
      char buf[1024];
      if(indent.length() != entries[i].m_level * 2)
        indent.assign(entries[i].m_level * 2, ' ');
      snprintf(buf, 1024, "%s[%s] called %d times, %g milliseconds.\n", indent.c_str(), entries[i].m_label, entries[i].m_invocations, entries[i].m_seconds * 1000.0);
      result += buf;
    }
    return result;
  }

private:

  bool m_enabled;
  std::vector< ProfilingInfo > m_profilingInfos;
  std::vector< unsigned int > m_parentKeyStack;
  unsigned int m_numProfiles;
  unsigned int m_stackEnd;
};

class AutoProfilingEvent
{
public:

  AutoProfilingEvent(char const * label, ProfilingStack & stack)
    : m_stack(stack)
  {
    m_index = stack.beginProfilingEvent(label);
  }

  ~AutoProfilingEvent()
  {
    m_stack.endProfilingEvent(m_index);
  }

  void stop()
  {
    m_stack.stopProfilingEvent(m_index);
  }

  void resume()
  {
    m_stack.resumeProfilingEvent(m_index);
  }

private:
  int m_index;
  ProfilingStack & m_stack;
};

FTL_NAMESPACE_END
