/** ========================================================================= *
 *
 * @file reset.h
 * @date 19-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief API for reset
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
/**
 * Reset method
 */
typedef enum {
  OS_RESET_HARD,    /** Hard Reset (if physically cutting power is supported) */
  OS_RESET_SOFT,    /** Soft Reset (usually done through NVIC_SystemReset) */
  OS_RESET_WDG,     /** WatchDog Reset (may be triggered to signal an error) */
} os_reset_method_t;

/**
 * Reset reason
 */
typedef enum {
  OS_RESET_REASON_UNK = 0,   /** Unknown reset reason */
  OS_RESET_REASON_HW_RST,    /** Hardware reset */
  OS_RESET_REASON_SW_RST,    /** Software reset */
  OS_RESET_REASON_WDG,       /** Reboot by watchdog */
  OS_RESET_REASON_WWDG,      /** Reboot by window watchdog */
  OS_RESET_REASON_POR,       /** Power On Reset */
  OS_RESET_REASON_BOR,       /** Brown Out Reset */
} os_reset_reason_t;

/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Resets the device, needs os_reset_port to function properly
 *
 * @param method Reboot method
 */
__NO_RETURN void os_reset(os_reset_method_t method);

/**
 * Port implementation of os_reset, BSP defined
 *
 * Has a default stub implementation
 *
 * @param method Reboot method
 */
__NO_RETURN void os_reset_port(os_reset_method_t method);

/**
 * Returns last reset reason
 */
os_reset_reason_t os_get_reset_reason(void);

/**
 * Port implementation of os_get_reset_reason, BSP defined
 *
 * Has a default stub implementation
 */
os_reset_reason_t os_get_reset_reason_port(void);

/**
 * Converts os_reset_method_t enum value to string
 *
 * @param method Reboot method
 */
const char * os_reset_method_to_str(os_reset_method_t method);

/**
 * Converts os_reset_reason_t enum value to string
 *
 * @param reason Reboot reason
 */
const char * os_reset_reason_to_str(os_reset_reason_t reason);

#ifdef __cplusplus
}
#endif