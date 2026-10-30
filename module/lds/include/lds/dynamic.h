/**
 * @brief Dynamic LDS implementation. Uses constructors
 *
 * Uses an __attribute__((constructor)) to achieve what looks like linker
 * sections, without the need to put anything into said linker sections
 *
 * Useful when target doesn't support user LD scripts, or has strict
 * restrictions on them (like most Host platforms).
 */
#pragma once

#include <lds/base.h>

__STATIC_ASSERT(__LDS_INCLUDE_MARKER, "Don't include this file directly, use lds.h");

/**
 * @brief Declare an LDS named `__name` with type `__type`
 * @note Call in once a header to make it accessible across everything that includes it
 */
#define LDS_DECLARE(__name, __type) \
  typedef __type LDS_TYPE_NAME(__name);                                       \
  extern __UNUSED LDS_TYPE_NAME(__name) * LDS_START_NAME(__name);             \
  extern __UNUSED LDS_TYPE_NAME(__name) * LDS_END_NAME(__name)

/**
 * @brief Define an LDS named `__name`
 * @note Call once in a .c file
 * @note LDS_DECLARE must be called prior
 * @note An optional second argument may be specified to set an upper limit
 *       to the size of the section. Default is LDS_MAX_SIZE
 */
#define LDS_DEFINE(__name, ...) \
  static LDS_TYPE_NAME(__name) UTIL_CAT(__name, _data)[                       \
    UTIL_DEFAULT_OR_VA_ARGS(LDS_MAX_SIZE, __VA_ARGS__)                        \
  ] = {0};                                                                    \
  LDS_TYPE_NAME(__name) * LDS_START_NAME(__name) = UTIL_CAT(__name, _data);   \
  LDS_TYPE_NAME(__name) * LDS_END_NAME(__name) = UTIL_CAT(__name, _data)

/**
 * @brief Base for all LDS_APPEND_* macros
 */
#define LDS_APPEND_BASE(__name, __item, __modifier, ...) \
  __USED __modifier const LDS_TYPE_NAME(__name) __item = __VA_ARGS__;         \
  __CONSTRUCTOR(255) void UTIL_CAT(_lds_init_, __item)() {                    \
    *LDS_END_NAME(__name) = __item;                                           \
    LDS_END_NAME(__name)++;                                                   \
  }

/**
 * @brief Get a pointer to the start of an LDS
 */
#define LDS_START(__name) \
  LDS_START_NAME(__name)

/**
 * @brief Get a pointer to the end of an LDS
 */
#define LDS_END(__name) \
  LDS_END_NAME(__name)
