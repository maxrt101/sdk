/**
 * @brief Static implementation of LDS. Uses linker sections
 *
 * @note For this to work lds.ld linker script needs to be included into
 *       your target's LD script, somewhere where DATA lives
 *
 * Uses a clever trick with linker SORT() directive to create a .0/.1/.2
 * suffixed subsections for each declared LDS to get size of a section
 * (start & end pointers) by placing them into .0 (start) and .2 (end)
 * and items into .1, which eliminates a per-LDS linker section definition
 * and __start/__stop linker symbols.
 */
#pragma once

#include <lds/base.h>

__STATIC_ASSERT(__LDS_INCLUDE_MARKER, "Don't include this file directly, use lds.h");

#define LDS_START_INDEX 0
#define LDS_ITEM_INDEX 1
#define LDS_END_INDEX 2

// TODO: Is MacOS case needed since there's no sorting and it wont work regardless?
#ifdef __APPLE__
#define LDS_SECTION_NAME(__name, __idx) "__DATA,.lds." UTIL_STRINGIFY(__name) "." UTIL_STRINGIFY(__idx)
#else
#define LDS_SECTION_NAME(__name, __idx) ".lds." UTIL_STRINGIFY(__name) "." UTIL_STRINGIFY(__idx)
#endif

/**
 * @brief Declare an LDS named `__name` with type `__type`
 * @note Call in once a header to make it accessible across everything that includes it
 */
#define LDS_DECLARE(__name, __type) \
  typedef __type LDS_TYPE_NAME(__name);                                       \
  extern __UNUSED const LDS_TYPE_NAME(__name) LDS_START_NAME(__name)[0];      \
  extern __UNUSED const LDS_TYPE_NAME(__name) LDS_END_NAME(__name)[0]

/**
 * @brief Define an LDS named `__name`
 * @note Call once in a .c file
 * @note LDS_DECLARE must be called prior
 */
#define LDS_DEFINE(__name, ...) \
  __WEAK __SECTION(LDS_SECTION_NAME(__name, LDS_START_INDEX))                 \
      const LDS_TYPE_NAME(__name) LDS_START_NAME(__name)[0];                  \
  __WEAK __SECTION(LDS_SECTION_NAME(__name, LDS_END_INDEX))                   \
      const LDS_TYPE_NAME(__name) LDS_END_NAME(__name)[0];                    \

/**
 * @brief Base for all LDS_APPEND_* macros
 */
#define LDS_APPEND_BASE(__name, __item, __modifier, ...) \
  __USED __modifier LDS_TYPE_NAME(__name) __item                              \
    __SECTION(LDS_SECTION_NAME(__name, LDS_ITEM_INDEX)) = __VA_ARGS__

/**
 * @brief Get a pointer to the start of an LDS
 */
#define LDS_START(__name) \
  ((LDS_TYPE_NAME(__name) *) &LDS_START_NAME(__name))

/**
 * @brief Get a pointer to the end of an LDS
 */
#define LDS_END(__name) \
  ((LDS_TYPE_NAME(__name) *) &LDS_END_NAME(__name))

