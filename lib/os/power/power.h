/** ========================================================================= *
 *
 * @file power.h
 * @date 27-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Power management API (sleep modes, etc)
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/compiler.h"
#include <stdbool.h>
#include <stddef.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 *
 *
 * @param __mode
 */
#define OS_WITH_POWER_MODE_BLOCKED(__mode) \
  for (bool s = os_power_mode_blocked_enter(__mode); s; s = os_power_mode_blocked_exit(__mode))

/* Enums ==================================================================== */
/* Types ==================================================================== */
typedef enum {
  OS_POWER_MODE_AUTO = 0,
  OS_POWER_MODE_FAST_SLEEP,
  OS_POWER_MODE_DEEP_SLEEP,

#if OS_POWER_MODE_PORT
  OS_POWER_MODE_PORT
#endif

  OS_POWER_MODE_COUNT,
} os_power_mode_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Change current power mode
 *
 * @param mode Power mode
 */
error_t os_power_mode_change(os_power_mode_t mode);

/**
 * Skips next power mode change for specific power mode
 *
 * @param mode Power mode
 */
error_t os_power_mode_skip_next(os_power_mode_t mode);

/**
 * Block transition to specific power mode until unblocked
 *
 * @param mode Power mode
 */
error_t os_power_mode_block(os_power_mode_t mode, bool block);

/**
 * Change current power mode
 *
 * @param mode Power mode
 */
error_t os_power_mode_change_port(os_power_mode_t mode);

/**
 * Converts power_mode_t enum value to string
 *
 * @param mode Power mode
 */
const char * os_power_mode_to_str(os_power_mode_t mode);

/**
 *
 *
 * @param mode
 */
__STATIC_INLINE bool os_power_mode_blocked_enter(os_power_mode_t mode) {
  os_power_mode_block(mode, true);
  return true;
}

/**
 *
 *
 * @param mode
 */
__STATIC_INLINE bool os_power_mode_blocked_exit(os_power_mode_t mode) {
  os_power_mode_block(mode, false);
  return false;
}

#ifdef __cplusplus
}
#endif
