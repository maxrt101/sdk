#pragma once

#define ASSERT_PANIC(...) \
  _Static_assert(false, "ASSERT_PANIC not implemented")

#define ASSERT_RETURN(__expr, __VA_ARGS__) \
  do {                                                                        \
    if (!(__expr)) {                                                          \
      return __VA_ARGS__;                                                     \
    }                                                                         \
  } while (0)

#define ERROR_CHECK_PANIC(...) \
  _Static_assert(false, "ERROR_CHECK_PANIC not implemented")

#define ERROR_CHECK_RETURN_IMPL(__expr, __var, ...) \
  ({ \
    error_t __var = __expr;\
    if (__var != E_OK) { \
      return __var; \
    } \
  })

#define ERROR_CHECK_RETURN(__expr, ...) \
  ERROR_CHECK_RETURN_IMPL(__expr, UTIL_CAT(_res, __LINE__), __VA_ARGS__)
