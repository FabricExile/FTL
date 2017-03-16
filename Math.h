//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FTL/Config.h>

#include <stdint.h>

FTL_NAMESPACE_BEGIN

template<typename T> T NaNValue();

template<> inline float NaNValue<float>()
{
  static uint32_t uint32Value = ~uint32_t(0);
  return *reinterpret_cast<float const *>( &uint32Value );
}

template<> inline double NaNValue<double>()
{
  static uint64_t uint64Value = ~uint64_t(0);
  return *reinterpret_cast<double const *>( &uint64Value );
}

inline bool IsInf( float value )
{
  uint32_t uint32Rep = *reinterpret_cast<uint32_t const *>( &value );
  return (uint32Rep & UINT32_C(0x7FFFFFFF)) == UINT32_C(0x7F800000);
}

inline bool IsInf( double value )
{
  uint64_t uint64Rep = *reinterpret_cast<uint64_t const *>( &value );
  return (uint64Rep & UINT64_C(0x7FFFFFFFFFFFFFFF)) == UINT64_C(0x7FF0000000000000);
}

inline bool IsNaN( float value )
{
  uint32_t uint32Rep = *reinterpret_cast<uint32_t const *>( &value );
  return (uint32Rep & UINT32_C(0x7F800000)) == UINT32_C(0x7F800000)
      && (uint32Rep & UINT32_C(0x007FFFFF)) != UINT32_C(0x00000000);
}

inline bool IsNaN( double value )
{
  uint64_t uint64Rep = *reinterpret_cast<uint64_t const *>( &value );
  return (uint64Rep & UINT64_C(0x7FF0000000000000)) == UINT64_C(0x7FF0000000000000)
      && (uint64Rep & UINT64_C(0x000FFFFFFFFFFFFF)) != UINT64_C(0x0000000000000000);
}

FTL_NAMESPACE_END
