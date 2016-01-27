/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#ifndef _FTL_Timer_h
#define _FTL_Timer_h

#include <FTL/Config.h>
#include <FTL/Ticks.h>

#include <stdio.h>

FTL_NAMESPACE_BEGIN

class Timer
{
public:

  Timer( char const *desc )
    : m_desc( desc )
    , m_offset( 0.0 )
    , m_begin( GetCurrentTicks() )
  {
  }

  ~Timer()
  {
    printf( "%s %0.3lfms\n", m_desc, getElapsedMS() );
  }

  double getElapsedMS( bool reset = false )
  {
    uint64_t currentTicks = GetCurrentTicks();
    double elapsed =
      m_offset + GetSecondsBetweenTicks( m_begin, currentTicks ) * 1e3;
    if ( reset )
      m_begin = currentTicks;
    return elapsed;
  }

  void pause()
  {
    m_offset +=
      GetSecondsBetweenTicks( m_begin, GetCurrentTicks() ) * 1e3;
  }

  void resume()
  {
    m_begin = GetCurrentTicks();
  }

private:

  char const *m_desc;
  double m_offset;
  uint64_t m_begin;
};

FTL_NAMESPACE_END

#endif //_FTL_Timer_h
