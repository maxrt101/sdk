/**
 * @brief Simple library for organizing const data into custom linker sections,
 *        hence LDS - LD(linker) Sections
 *
 * Includes 2 implementations: static & dynamic:
 *  - Static will use actual LD sections
 *  - Dynamic will implement the same API, but will utilize attribute((constructor))
 *    to emulate LD sections on platforms, where they are not supported (e.g. Host)
 *
 * Dynamic implementation is used by default, to change it, override LDS_DYNAMIC feature to 0
 * and include lds.ld file into .data section in your target's linker script
 */

#pragma once

/**
 * @brief Special marker to prevent inclusion of static.h/dynamic.h by themselves
 */
#define __LDS_INCLUDE_MARKER 1

#if LDS_DYNAMIC
#include <lds/dynamic.h>
#else
#include <lds/static.h>
#endif

#undef __LDS_INCLUDE_MARKER

/**
 * @brief Append a named item to an LDS
 *
 * Example:
 * @code{.c}
    struct data { int num; int cnt; };
    LDS_DECLARE(test, struct data);
    LDS_APPEND_NAMED(test, item_1, { .num = 1, .cnt = 42 });
 * @endcode
 */
#define LDS_APPEND_NAMED(__name, __item, ...) \
  LDS_APPEND_BASE(__name, __item, , __VA_ARGS__)

/**
 * @brief Same as `LDS_APPEND_NAMED`, but the resulting item is static
 */
#define LDS_APPEND_NAMED_STATIC(__name, __item, ...) \
  LDS_APPEND_BASE(__name, __item, static, __VA_ARGS__)

/**
 * @brief Append an item to an LDS
 *
 * Example:
 * @code{.c}
    struct data { int num; int cnt; };
    LDS_DECLARE(test, struct data);
    LDS_APPEND_NAMED(test, { .num = 1, .cnt = 42 });
 * @endcode
 */
#define LDS_APPEND(__name, ...) \
  LDS_APPEND_BASE(__name, LDS_UNIQUE_NAME(__name), , __VA_ARGS__)

/**
 * @brief Same as `LDS_APPEND` but the resulting item is static
 */
#define LDS_APPEND_STATIC(__name, ...) \
  LDS_APPEND_BASE(__name, LDS_UNIQUE_NAME(__name), static, __VA_ARGS__)

/**
 * @brief Call in a header to export named(!) LDS item
 * @note Only works for items declared with `LDS_APPEND_NAMED`
 */
#define LDS_EXPORT_ITEM(__name, __item) \
  extern const LDS_TYPE_NAME(__name) __item

/**
 * @brief Iterate over items in an LDS
 * @note Can be treated as a for-loop (which it is)
 *
 * Example:
 * @code{.c}
    LDS_DECLARE(items, int);
    LDS_FOR_EACH(items, i) {
      printf("%d\n", i);
    }
 * @endcode
 */
#define LDS_FOR_EACH(__name, __it) \
  for (LDS_TYPE_NAME(__name) * __it = LDS_START(__name); __it < LDS_END(__name); ++__it)

/**
 * @brief Get an item by a predicate
 *
 * Example:
 * @code{.c}
    LDS_DECLARE(items, int);
    #define PRED(__x) __x == 42
    int forty_two = *LDS_GET_BY_PRED(items, PRED);
 * @endcode
 */
#define LDS_GET_BY_PRED(__name, __pred) \
  ({                                                                          \
    LDS_TYPE_NAME(__name) * item = NULL;                                      \
    LDS_FOR_EACH(__name, _x) {                                                \
      if (__pred(_x)) {                                                       \
        item = _x;                                                            \
      }                                                                       \
    }                                                                         \
    item;                                                                     \
  })

/**
 * @brief Get an item by a equality check
 *
 * Example:
 * @code{.c}
    LDS_DECLARE(items, int);
    int forty_two = *LDS_GET_EQ(items, 42);
 * @endcode
 */
#define LDS_GET_EQ(__name, __val) \
  ({                                                                          \
    LDS_TYPE_NAME(__name) * item = NULL;                                      \
    LDS_FOR_EACH(__name, _x) {                                                \
      if (*_x == __val) {                                                     \
        item = _x;                                                            \
      }                                                                       \
    }                                                                         \
    item;                                                                     \
  })

/**
 * @brief Get an item by a equality member check
 *
 * Example:
 * @code{.c}
    struct test { int cnt; };
    LDS_DECLARE(items, struct test);
    int forty_two = LDS_GET_EQ_MEMBER(items, cnt, 42)->cnt;
 * @endcode
 */
#define LDS_GET_EQ_MEMBER(__name, __member, __val) \
  ({                                                                          \
    LDS_TYPE_NAME(__name) * item = NULL;                                      \
    LDS_FOR_EACH(__name, _x) {                                                \
      if (_x->__member == __val) {                                            \
        item = _x;                                                            \
      }                                                                       \
    }                                                                         \
    item;                                                                     \
  })
