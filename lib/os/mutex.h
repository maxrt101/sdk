/** ========================================================================= *
*
 * @file mutex.h
 * @date 28-02-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "os.h"

/* Defines ================================================================== */
/**
 * Size of waiters list in mutex
 */
#ifndef OS_MUTEX_MAX_WAITERS
#define OS_MUTEX_MAX_WAITERS 4
#endif

/**
 * If enabled, aborts if waiters list is full, but another lock is requested
 */
#ifndef OS_MUTEX_ABORT_ON_WAITER_OVERFLOW
#define OS_MUTEX_ABORT_ON_WAITER_OVERFLOW 0
#endif

/**
 * If enabled, will trace all mutex operations to log_debug
 */
#ifndef USE_OS_TRACE_MUTEX
#define USE_OS_TRACE_MUTEX 1
#endif

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Mutex context
 */
typedef struct {
  enum {
    OS_MUTEX_UNLOCKED = 0,
    OS_MUTEX_LOCKED   = 1,
  } status;
  os_task_t * owner;
  os_task_t * waiters[OS_MUTEX_MAX_WAITERS];
} os_mutex_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes mutex
 *
 * @note By default, sets owner to current task
 *
 * @param mutex Mutex handle
 */
void os_mutex_init(os_mutex_t * mutex);

/**
 * Resets mutex (clears waiters list)
 *
 * @param mutex Mutex handle
 */
void os_mutex_reset(os_mutex_t * mutex);

/**
 * Locks mutex
 *
 * Returns immediately, if mutex is unlocked, sets owner to current task
 * If mutex is locked, will delay until timeout is passed, and call try_lock
 * If timeout is NULL, will lock task (pause indefinitely) until mutex is unlocked
 *
 * @warning May cause deadlock if called with timeout == NULL
 *
 * @param mutex Mutex handle
 * @param timeout Timeout to wait for mutex to unlock, if locked. NULL to wait forever
 */
bool os_mutex_lock(os_mutex_t * mutex, timeout_t * timeout);

/**
 * Tries to lock mutex
 *
 * If mutex is unlocked, locks it, and sets owner to current task
 * If mutex is locked, returns
 *
 * @param mutex Mutex handle
 * @retval true If locked successfully
 * @retval false If already locked (lock failed)
 */
bool os_mutex_try_lock(os_mutex_t * mutex);

/**
 * Unlocks mutex
 *
 * If already unlocked - does nothing
 * Notifies each locked task by changing LOCKED to WAITING and setting increasing timeout in order of lock requests
 *
 * @param mutex Mutex handle
 */
void os_mutex_unlock(os_mutex_t * mutex);

#ifdef __cplusplus
}
#endif