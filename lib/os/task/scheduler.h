/** ========================================================================= *
 *
 * @file scheduler.h
 * @date 16-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Preemptive task scheduler
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "os/task/task.h"
#include "util/util.h"

/* Defines ================================================================== */
#ifndef OS_IS_SCHEDULE_IRQ_NAKED
#define OS_IS_SCHEDULE_IRQ_NAKED 0
#endif

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
extern volatile uint8_t os_scheduler_started;

/* Shared functions ========================================================= */
/**
 * Starts scheduler
 *
 * Should be ran once from single-task context (like main or Reset_Handler)
 *
 * @note At least one task must be present
 */
void os_launch(void);

/**
 * Called by os_schedule, performs the actual task switch
 *
 * Shouldn't be called on it's own, use os_schedule() instead
 *
 * @note At least one task must be created
 * @note os_launch must be called at some point to start the scheduler
 * @note This function is __NAKED to remove any callee/caller prologue/epilogue
 *       code
 */
__NAKED void os_schedule_impl(void);

/**
 * Switches tasks. Performs context switches as long as the scheduler is active
 *
 * @note Can be called from any periodic IRQ
 * @note This function is __STATIC_FORCEINLINE to embed it into the caller and
 *       remove any prologue/epilogue code
 */
__STATIC_FORCEINLINE void os_schedule(void) {
  if (os_scheduler_started) {
    os_schedule_impl();
  }
}

/**
 * Called when task exited
 *
 * TODO: pass os_task_t * pointer of the task that exited
 * TODO: Figure out how to get this called, upon return,
 *       if GCC returns by 'pop {R4, PC}'
 */
void os_task_exited(void);

#ifdef __cplusplus
}
#endif