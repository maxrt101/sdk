#pragma once

#include <util.h>

/**
 * @brief Generate a type name from section name
 */
#define LDS_TYPE_NAME(__name) UTIL_CAT_ALL(_lds_, __name, _t)

/**
 * @brief Generate a name of a variable that points to the start of the section data
 */
#define LDS_START_NAME(__name) UTIL_CAT(_lds_start, __name)

/**
 * @brief Generate a name of a variable that points to the end of the section data
 */
#define LDS_END_NAME(__name) UTIL_CAT(_lds_end, __name)

#if 0
/**
 * @brief Generate a unique name for an LDS item
 * @note It's used because __COUNTER__ updates every invocation, so it's unusable in dynamic impl
 *       but potentially could be used when LDS_DYNAMIC=0
 * @todo Investigate, as it can be used, if passed to LDS_APPEND_NAMED called by LDS_APPEND
 */
#define LDS_UNIQUE_NAME(__name) UTIL_CAT_ALL(_lds_item_, __name, _, __COUNTER__, __LINE__)
#else
/**
 * @brief Generate a unique name for an LDS item
 * @note Has a bug - if in 2 separate files on the same line LDS_APPEND in called -
 *       will fail, since uniqueness is achieved by using __LINE__. However the issue
 *       is pretty hard to encounter and can be easily worked around
 */
#define LDS_UNIQUE_NAME(__name) UTIL_CAT_ALL(_lds_item_, __name, _, __LINE__)
#endif
