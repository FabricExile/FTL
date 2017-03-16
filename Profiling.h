/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Ticks.h>
#include <FTL/OrderedStringMap.h>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <stdio.h>

FTL_NAMESPACE_BEGIN

class ProfilingInfo
{
public:

  ProfilingInfo(char const * label, int parentIndex)
    : m_label(label)
    , m_parentIndex(parentIndex)
    , m_begin(GetCurrentTicks())
    , m_elapsed(0.0)
    , m_paused(false)
  {
    m_end = m_begin;
  }

  ProfilingInfo()
    : m_label(NULL)
    , m_parentIndex(-1)
    , m_begin(0)
    , m_end(0)
    , m_elapsed(0.0)
    , m_paused(false)
  {
  }

  ProfilingInfo(const ProfilingInfo & other)
    : m_label(other.m_label)
    , m_parentIndex(other.m_parentIndex)
    , m_begin(other.m_begin)
    , m_end(other.m_end)
    , m_elapsed(other.m_elapsed)
    , m_paused(other.m_paused)
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
  bool m_paused;
};

struct ProfilingEntry
{
  ProfilingEntry(char const * label)
  : m_label(label)
  , m_index(0)
  , m_parentIndex(-1)
  , m_level(0)
  , m_invocations(0)
  , m_seconds(0.0)
  {
  }

  ProfilingEntry(const ProfilingEntry & other)
  : m_label(other.m_label)
  , m_callStack(other.m_callStack)
  , m_index(other.m_index)
  , m_parentIndex(other.m_parentIndex)
  , m_level(other.m_level)
  , m_invocations(other.m_invocations)
  , m_seconds(other.m_seconds)
  {
  }

  static bool compare(const ProfilingEntry & a, const ProfilingEntry & b)
  {
    return a.m_seconds > b.m_seconds;
  }

  char const * m_label;
  std::vector< FTL::CStrRef > m_callStack;
  int m_index;
  int m_parentIndex;
  unsigned m_level;
  unsigned m_invocations;
  double m_seconds;
};

class ProfilingStack
{
public:

  ProfilingStack(bool enabled = true, unsigned initialStackSize = 4096 * 1024)
    : m_enabled(enabled)
    , m_numProfiles(0)
    , m_stackLastIndex(-1)
  {
    assert(initialStackSize > 0);
    m_profilingInfos.resize(m_enabled ? initialStackSize : 0);
    m_parentKeyStack.resize(m_enabled ? 512 : 0);
  }

  int beginProfilingEvent(char const * label)
  {
    if(!m_enabled)
      return -1;

    int parentIndex = -1;
    if( m_stackLastIndex != -1 )
    {
      parentIndex = m_parentKeyStack[m_stackLastIndex];

      // only use parent indices from timers which are running.
      // this is done to avoid recursion.
      if(m_profilingInfos[parentIndex].m_paused)
      {
        parentIndex = -1;
      }
    }

    if(m_numProfiles == m_profilingInfos.size())
      m_profilingInfos.resize(m_profilingInfos.size() * 2);
    if(m_stackLastIndex == int(m_parentKeyStack.size()) - 1)
      m_parentKeyStack.resize(m_parentKeyStack.size() * 2);

    m_profilingInfos[m_numProfiles] = ProfilingInfo(label, parentIndex);
    m_parentKeyStack[++m_stackLastIndex] = m_numProfiles;
    m_numProfiles++;

    return m_numProfiles-1;
  }

  bool endProfilingEvent(int index)
  {
    if(!m_enabled)
      return false;
    
    uint64_t endTick = GetCurrentTicks();

    assert( index >= 0 && unsigned(index) < m_numProfiles );
    if( index < 0 || unsigned(index) >= m_numProfiles )
      return false;

    assert( m_stackLastIndex != -1 );
    if( m_stackLastIndex == -1 )
      return false;

    m_profilingInfos[index].m_end = endTick;
    m_stackLastIndex--;

    return true;
  }

  bool pauseProfilingEvent(int index)
  {
    if(!m_enabled)
      return false;

    uint64_t endTick = GetCurrentTicks();

    assert( index >= 0 && unsigned(index) < m_numProfiles );
    if( index < 0 || unsigned(index) >= m_numProfiles )
      return false;

    while(index != -1)
    {
      if(!m_profilingInfos[index].m_paused)
      {
        m_profilingInfos[index].m_elapsed += GetSecondsBetweenTicks(m_profilingInfos[index].m_begin, endTick);
        m_profilingInfos[index].m_begin = m_profilingInfos[index].m_end = endTick;
        m_profilingInfos[index].m_paused = true;
      }
      index = m_profilingInfos[index].m_parentIndex;
    }
    return true;
  }

  bool resumeProfilingEvent(int index)
  {
    if(!m_enabled)
      return false;

    uint64_t beginTick = GetCurrentTicks();

    assert( index >= 0 && unsigned(index) < m_numProfiles );
    if( index < 0 || unsigned(index) >= m_numProfiles )
      return false;

    while(index != -1)
    {
      if(m_profilingInfos[index].m_paused)
      {
        m_profilingInfos[index].m_begin = m_profilingInfos[index].m_end = beginTick;
        m_profilingInfos[index].m_paused = false;
      }
      index = m_profilingInfos[index].m_parentIndex;
    }
    return true;
  }

  std::vector< ProfilingEntry > getReportEntries() const
  {
    std::vector< ProfilingEntry > entries;
    OrderedStringMap< unsigned > lookup;

    for(size_t i=0;i<m_numProfiles;i++)
    {
      std::vector< FTL::CStrRef > callstack;
      callstack.push_back(m_profilingInfos[i].m_label);
      int parentIndex = m_profilingInfos[i].m_parentIndex;
      while(parentIndex >= 0)
      {
        callstack.push_back(m_profilingInfos[parentIndex].m_label);
        parentIndex = m_profilingInfos[parentIndex].m_parentIndex;
      }
      std::reverse(callstack.begin(), callstack.end());
      std::string callstackKey = getKeyForCallstack(callstack, callstack.size());
      
      OrderedStringMap< unsigned >::const_iterator it = lookup.find(callstackKey.c_str());
      if(it == lookup.end())
      {
        ProfilingEntry entry(m_profilingInfos[i].m_label);
        entry.m_index = (unsigned)entries.size();
        entry.m_parentIndex = -1;
        entry.m_callStack = callstack;
        entry.m_invocations = 1;
        entry.m_level = callstack.size() - 1;

        if(entry.m_level > 0)
        {
          std::string parentCallstackKey = getKeyForCallstack(callstack, callstack.size()-1);

          OrderedStringMap< unsigned >::const_iterator parentIt = lookup.find(parentCallstackKey.c_str());
          assert(parentIt != lookup.end());
          entry.m_parentIndex = parentIt - lookup.begin();
        }

        entry.m_seconds = m_profilingInfos[i].getSeconds();
        lookup.insert(callstackKey.c_str(), (unsigned)entries.size());
        entries.push_back(entry);
      }
      else
      {
        entries[it->second].m_seconds += m_profilingInfos[i].getSeconds();
        entries[it->second].m_invocations++;
      }
    }

    std::sort(entries.begin(), entries.end(), ProfilingEntry::compare);

    std::vector< ProfilingEntry > result;
    traverseEntries(-1, entries, result);

    return result;
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
      snprintf(buf, 1024, "%s[%s] called %d times, %.3g ms (%.3g ms).\n", indent.c_str(), entries[i].m_label, entries[i].m_invocations, entries[i].m_seconds * 1000.0, entries[i].m_seconds * 1000.0 / double(entries[i].m_invocations));
      result += buf;
    }
    return result;
  }

private:

  std::string getKeyForCallstack(const std::vector< FTL::CStrRef > & callstack, size_t numCalls) const
  {
    std::string result;
    for(size_t i=0;i<numCalls;i++)
    {
      if(result.length() > 0)
      {
        result += " -> ";
      }
      result += callstack[i].c_str();
    }
    return result;
  }

  void traverseEntries(int parentIndex, const std::vector< ProfilingEntry > & entries, std::vector< ProfilingEntry > & result) const
  { 
    for(size_t i = 0; i < entries.size(); i++)
    {
      if(entries[i].m_parentIndex != parentIndex)
        continue;
      result.push_back(entries[i]);
      traverseEntries(entries[i].m_index, entries, result);
    }
  }

  bool m_enabled;
  std::vector< ProfilingInfo > m_profilingInfos;
  std::vector< unsigned > m_parentKeyStack;
  unsigned m_numProfiles;
  int m_stackLastIndex;
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

  void pause()
  {
    m_stack.pauseProfilingEvent(m_index);
  }

  void resume()
  {
    m_stack.resumeProfilingEvent(m_index);
  }

private:
  int m_index;
  ProfilingStack & m_stack;
};

class AutoProfilingPauseEvent
{
public:

  AutoProfilingPauseEvent(AutoProfilingEvent & event)
    : m_event(event)
    , m_resumed(false)
  {
    m_event.pause();
  }

  ~AutoProfilingPauseEvent()
  {
    resume();
  }

  void resume()
  {
    if(m_resumed)
      return;
    m_event.resume();
    m_resumed = true;
  }


private:
  AutoProfilingEvent & m_event;
  bool m_resumed;
};

FTL_NAMESPACE_END
