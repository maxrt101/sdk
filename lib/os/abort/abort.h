/** ========================================================================= *
 *
 * @file abort.h
 * @date 20-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief API for abort (non-recoverable software exception)
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
 * Aborts application (show error msg and reboots by WDG)
 */
__NO_RETURN void os_abort(const char * msg, ...);

/**
 * Used defined callback to print abort context (like register states, etc.)
 */
void os_abort_dump_ctx(void);

#ifdef __cplusplus
}
#endif