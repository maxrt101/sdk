/** ========================================================================= *
 *
 * @file wdt.h
 * @date 18-04-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Generic WDT (WatchDogTimer) HAL API, has no default implementation
 *
 * Board must implement all functions in this file
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "util/compiler.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initialize (enable) WatchDogTimer
 */
void wdt_init(void);

/**
 * Reload WatchDogTimer counter
 */
void wdt_feed(void);

/**
 * Reset system by WatchDog
 */
__NORETURN void wdt_reboot(void);

#ifdef __cplusplus
}
#endif
