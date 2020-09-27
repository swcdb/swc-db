/*
 * SWC-DB© Copyright since 2019 Alex Kashirin <kashirin.alex@gmail.com>
 * License details at <https://github.com/kashirin-alex/swc-db/#license>
 */

#ifndef swc_core_COMPAT_H
#define swc_core_COMPAT_H

//! The SWC-DB C++ namespace 'SWC'
namespace SWC { }

#ifndef SWC_MALLOC_NOT_INSISTENT
#include "swcdb/core/Malloc.h"
#endif


#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <cstddef> // for std::size_t and std::ptrdiff_t
#include <memory>
#include <string>


#define SWC_ATTRIBS(attrs) __attribute__(attrs)
#define SWC_ATTR_NOTHROW __nothrow__
#define SWC_ATTR_INLINE __always_inline__ //, __artificial__

#define SWC_NOEXCEPT noexcept(true)


#ifdef SWC_IMPL_SOURCE
# define SWC_SHOULD_INLINE  \
  SWC_ATTRIBS((SWC_ATTR_INLINE)) \
  extern inline
#else 
# define SWC_SHOULD_INLINE
#endif


# define SWC_SHOULD_NOT_INLINE  \
  SWC_ATTRIBS((__noinline__, __noclone__))


# define SWC_CAN_INLINE  \
  SWC_ATTRIBS((SWC_ATTR_INLINE)) \
  inline




#if defined (__GLIBC__) && (__GLIBC__ >= 2) && (__GLIBC_MINOR__ >= 23)
#define USE_READDIR_R 0
#else
#define USE_READDIR_R 1
#endif

#ifdef SWC_USE_ABORT
#define SWC_ABORT abort()
#else
#define SWC_ABORT raise(SIGABRT)
#endif


#include "swcdb/core/BitFieldInt.h" // for u/int24/40/48/56_t

#endif
