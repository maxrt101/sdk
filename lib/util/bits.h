/** ========================================================================= *
 *
 * @file bits.h
 * @date 29-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Bit manipulation utilities
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "util/compiler.h"
#include "util/vargs.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Sets bit
 */
#define UTIL_BIT_SET(val, bit) (val | ((uint32_t) 1 << bit))

/**
 * Clears bit
 */
#define UTIL_BIT_CLEAR(val, bit) (val & ~((uint32_t) 1 << bit))

/**
 * Toggles bit
 */
#define UTIL_BIT_TOGGLE(val, bit) (val ^ ((uint32_t) 1 << bit))

/**
 * Gets bit
 */
#define UTIL_BIT_GET(val, bit) ((val & ((uint32_t) 1 << bit)) ? 1 : 0)

/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */

#ifdef __cplusplus
}
#endif
