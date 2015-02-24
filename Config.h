/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#ifndef _FabricOS_Config_h
#define _FabricOS_Config_h

#if defined( __linux__ )

# define FABRIC_OS_LINUX
# define FABRIC_PLATFORM_POSIX

#elif defined( __APPLE__ )

# define FABRIC_OS_DARWIN
# define FABRIC_PLATFORM_POSIX

#elif defined( _MSC_VER )

# define FABRIC_OS_WINDOWS
# define FABRIC_PLATFORM_WINDOWS

# if !defined( FABRIC_INCLUDE_WINDOWS_UI )
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
static inline double round( double x ) { return floor(x+0.5); }
static inline float roundf( float x ) { return floorf(x+0.5f); }
#define snprintf _snprintf

#else
# error Unknown platform.
#endif

// Build architecture
#if defined(FABRIC_PLATFORM_POSIX)
# if defined(__x86_64)
#  define FABRIC_ARCH_64BIT
# else
#  define FABRIC_ARCH_32BIT
# endif
#elif defined(FABRIC_PLATFORM_WINDOWS)
# if defined(_WIN64)
#  define FABRIC_ARCH_64BIT
# else
#  define FABRIC_ARCH_32BIT
# endif
#else
# error "Unsupported platform"
#endif

// Build settings
#if defined(NDEBUG)
# define FABRIC_BUILD_RELEASE
#else
# define FABRIC_BUILD_DEBUG
#endif

#endif // _FabricOS_Config_h
