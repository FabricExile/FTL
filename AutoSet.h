/*
 *  Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
 */

#pragma once

#include <FTL/Config.h>

FTL_NAMESPACE_BEGIN

template< class T > class AutoSet
{
public:

  AutoSet( T &lValue, T newRValue )
    : m_lValue( lValue )
    , m_oldRValue( lValue )
  {
    m_lValue = newRValue;
  }

  ~AutoSet()
  {
    m_lValue = m_oldRValue;
  }

private:

  T &m_lValue;
  T m_oldRValue;
};

FTL_NAMESPACE_END
