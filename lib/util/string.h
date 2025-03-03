/** ========================================================================= *
 *
 * @file string.h
 * @date 23-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Preprocessor utilities that involve strings
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <string.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Converts token to string literal
 */
#define UTIL_STRINGIFY(str) #str

/**
 * Safe string copy (truncates source, if destination is smaller than source)
 */
#define UTIL_STR_COPY(dst, src, max_size) \
    memcpy(dst, src, UTIL_MIN(max_size, strlen(src)))

/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */

#ifdef __cplusplus
}
#endif