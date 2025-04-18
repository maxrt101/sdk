/** ========================================================================= *
*
 * @file soft_wdg.h
 * @date 18-04-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Software WatchDog
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
  SWDG_ACTION_REBOOT_SOFT  = (1 << 0),
  SWDG_ACTION_REBOOT_HARD  = (1 << 1),
  SWDG_ACTION_REBOOT_WDT   = (1 << 2),
  SWDG_ACTION_NOTIFY       = (1 << 3),
  SWDG_ACTION_LOG          = (1 << 4),

  SWDG_ACTION_DEFAULT      = SWDG_ACTION_REBOOT_WDT | SWDG_ACTION_NOTIFY | SWDG_ACTION_LOG,
} soft_wdg_action_t;

/* Types ==================================================================== */
/**
 * Software WatchDog Context
 */
typedef struct {
  const char * label;
  void * ctx;
  uint32_t counter;
  uint32_t max;
  uint8_t action;
} soft_wdg_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Init WatchDogTimer
 *
 * @param wdg SoftWDG Context
 * @param max Max value, that counter can reach before being considered expired
 * @param action Bitmask of actions to perform
 */
void soft_wdg_init(soft_wdg_t * wdg, uint32_t max, uint8_t action, const char * label, void * ctx);

/**
 * Update WatchDog counter
 *
 * @param wdg SoftWDG Context
 */
void soft_wdg_inc(soft_wdg_t * wdg);

/**
 * Reset WatchDog counter
 *
 * @param wdg SoftWDG Context
 */
void soft_wdg_reset(soft_wdg_t * wdg);

/**
 * Called when counter expired
 *
 * @param wdg SoftWDG Context
 */
void soft_wdg_on_timeout(soft_wdg_t * wdg);

#ifdef __cplusplus
}
#endif