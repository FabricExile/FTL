/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Ticks.h>
#include <map>
#include <vector>

FTL_NAMESPACE_BEGIN

class ProfilingInfo
{
public:

  ProfilingInfo(char const * label, int parentIndex)
    : m_label(label)
    , m_parentIndex(parentIndex)
    , m_begin( GetCurrentTicks() )
  {
    m_end = m_begin;
  }

  ProfilingInfo()
    : m_label(NULL)
    , m_parentIndex(-1)
    , m_begin(0)
    , m_end(0)
  {
  }

  ProfilingInfo(const ProfilingInfo & other)
    : m_label(other.m_label)
    , m_parentIndex(other.m_parentIndex)
    , m_begin(other.m_begin)
    , m_end(other.m_end)
  {
  }

  double getSeconds() const
  {
    return GetSecondsBetweenTicks(m_begin, m_end);
  }

  double getMS() const
  {
    return getSeconds() * 1000.0;
  }

  char const *m_label;
  int m_parentIndex;
  uint64_t m_begin;
  uint64_t m_end;
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
      parentIndex = m_parentKeyStack[m_stackEnd-1];

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

    printf("%s duration %fms\n", m_profilingInfos[index].m_label, (float)m_profilingInfos[index].getMS());

    return true;
  }

private:

  bool m_enabled;
  std::vector< ProfilingInfo > m_profilingInfos;
  std::vector< unsigned int > m_parentKeyStack;
  unsigned int m_numProfiles;
  unsigned int m_stackEnd;
};

class AutoProfilingBracket
{
public:

  AutoProfilingBracket(char const * label, ProfilingStack & stack)
    : m_stack(stack)
  {
    m_index = stack.beginProfilingEvent(label);
  }

  ~AutoProfilingBracket()
  {
    m_stack.endProfilingEvent(m_index);
  }

private:
  int m_index;
  ProfilingStack & m_stack;
};

FTL_NAMESPACE_END
