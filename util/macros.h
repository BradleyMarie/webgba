#ifndef _WEBGBA_UTIL_MACROS_
#define _WEBGBA_UTIL_MACROS_

#include <assert.h>

#ifdef NDEBUG
#define codegen_assert(condition)              \
  do {                                         \
    if (!(condition)) __builtin_unreachable(); \
  } while (0)
#else
#define codegen_assert(condition) assert(condition)
#endif

#endif  // _WEBGBA_UTIL_MACROS_