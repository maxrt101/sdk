/** ========================================================================= *
 *
 * @file soft_wdt.h
 * @date 18-04-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Software WatchDogTimer
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "time/timeout.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
typedef enum {
  SWDT_ACTION_REBOOT_SOFT  = (1 << 0),
  SWDT_ACTION_REBOOT_HARD  = (1 << 1),
  SWDT_ACTION_REBOOT_WDT   = (1 << 2),
  SWDT_ACTION_NOTIFY       = (1 << 3),
  SWDT_ACTION_LOG          = (1 << 4),

  SWDT_ACTION_DEFAULT      = SWDT_ACTION_REBOOT_WDT | SWDT_ACTION_NOTIFY | SWDT_ACTION_LOG,
} soft_wdt_action_t;

/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Init WatchDogTimer
 *
 * @param timeout Timeout
 * @param action Bitmask of actions to perform
 */
void soft_wdt_init(milliseconds_t timeout, uint8_t action, void * ctx);

/**
 * Reload WatchDogTimer timeout
 *
 */
void soft_wdt_feed(void);

/**
 * Check WatchDogTimer timeout
 */
void soft_wdt_check(void);

/**
 * Called when timeout expired
 */
void soft_wdt_on_timeout(void * ctx);

#ifdef __cplusplus
}
#endif