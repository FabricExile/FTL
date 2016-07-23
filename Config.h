/*
 *  Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
 */

#pragma once

#if defined( __linux__ )

# define FTL_OS_LINUX
# define FTL_PLATFORM_POSIX

#elif defined( __APPLE__ )

# define FTL_OS_DARWIN
# define FTL_PLATFORM_POSIX

#elif defined( _MSC_VER )

# define FTL_OS_WINDOWS
# define FTL_PLATFORM_WINDOWS

# if !defined( FTL_INCLUDE_WINDOWS_UI )
#  define WIN32_LEAN_AND_MEAN   // Avoid unnecessary cruft
# endif

# if !defined( WIN32 ) //Note: WIN32 is defined for 64 bits too however WIN64 is only defined in 64 bits.
#  define WIN32 1
# endif

// For some reason, this warning cannot be disabled
// from the command line.
// 'Unsupported zero-sized arrays in structs blah...'
#pragma warning( disable: 4200 )


// Some missing POSIX-y functions
#include <math.h>
inline double round( double x ) { return floor(x+0.5); }
inline float roundf( float x ) { return floorf(x+0.5f); }
#define snprintf _snprintf

#else
# error Unknown platform.
#endif

// Build architecture
#if defined(FTL_PLATFORM_POSIX)
# if defined(__x86_64)
#  define FTL_ARCH_64BIT
# else
#  define FTL_ARCH_32BIT
# endif
#elif defined(FTL_PLATFORM_WINDOWS)
# if defined(_WIN64)
#  define FTL_ARCH_64BIT
# else
#  define FTL_ARCH_32BIT
# endif
#else
# error "Unsupported platform"
#endif

// Build settings
#if defined(NDEBUG)
# define FTL_BUILD_RELEASE
#else
# define FTL_BUILD_DEBUG
#endif

#ifndef _MSC_VER
# define FTL_NOEXCEPT noexcept
#else
# define FTL_NOEXCEPT
#endif

#ifndef __has_feature
# define __has_feature(x) 0
#endif

// From LLVM 3.3 source
#if (__has_feature(cxx_rvalue_references)   \
     || defined(__GXX_EXPERIMENTAL_CXX0X__) \
     || (defined(_MSC_VER) && _MSC_VER >= 1600))
# define FTL_HAS_RVALUE_REFERENCES 1
#else
# define FTL_HAS_RVALUE_REFERENCES 0
#endif

// From LLVM 3.3 source
#if (__has_feature(cxx_deleted_functions) \
     || defined(__GXX_EXPERIMENTAL_CXX0X__))
     // No version of MSVC currently supports this.
# define FTL_DELETED_FUNCTION = delete
#else
# define FTL_DELETED_FUNCTION
#endif

#if !defined(FTL_NAMESPACE)
# define FTL_NAMESPACE FTL
#endif

#define FTL_NAMESPACE_BEGIN namespace FTL_NAMESPACE {
#define FTL_NAMESPACE_END   }
