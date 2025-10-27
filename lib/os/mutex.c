/** ========================================================================= *
*
 * @file mutex.c
 * @date 27-02-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "log/log.h"
#include "error/assertion.h"
#include "os/abort/abort.h"
#include "os.h"
#include "mutex.h"

/* Defines ================================================================== */
#define LOG_TAG os

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
void os_mutex_init(os_mutex_t * mutex) {
  ASSERT_RETURN(mutex);

  memset(mutex, 0, sizeof(*mutex));

  // By default, task that created the mutex, will be it's first owner
  mutex->owner = os_task_current();

  OS_LOG_TRACE(MUTEX, "mutex init %p (owner %p '%s')", mutex, mutex->owner, mutex->owner->name);
}

void os_mutex_reset(os_mutex_t * mutex) {
  ASSERT_RETURN(mutex);

  // Reset waiter list
  // FIXME: Maybe need to notify waiters?
  memset(mutex->waiters, 0, sizeof(mutex->waiters));

  OS_LOG_TRACE(MUTEX, "mutex reset %p", mutex);
}

bool os_mutex_lock(os_mutex_t * mutex, timeout_t * timeout) {
  ASSERT_RETURN(mutex, false);

  OS_LOG_TRACE(MUTEX, "os_mutex_lock: %p (owner %p '%s') by %p '%s' for %d ms",
    mutex, mutex->owner, mutex->owner->name,
    os_task_current(), os_task_current()->name,
    timeout ? timeout->duration : -1);

  if (mutex->status == OS_MUTEX_LOCKED) {
    if (mutex->owner == os_task_current()) {
      // If mutex is locked by current task - return, nothing is needed to be done
      OS_LOG_TRACE(MUTEX, "os_mutex_lock: %p already locked by %p '%s', locking considered successful",
        mutex, os_task_current(), os_task_current()->name);
      return true;
    }

    bool task_added_to_waiters = false;

    // Add current task to waiters
    for (uint8_t i = 0; i < OS_MUTEX_MAX_WAITERS; ++i) {
      if (!mutex->waiters[i]) {
        mutex->waiters[i] = os_task_current();
        task_added_to_waiters = true;
        break;
      }
    }

    // It current task wasn't added because waiters list is full - abort or return false
    if (!task_added_to_waiters) {
      UTIL_IF_1(OS_MUTEX_ABORT_ON_WAITER_OVERFLOW,
        os_abort("os_mutex_lock: waiter list overflow for mutex %p (owner %p '%s'), lock tried by %p '%s'",
          mutex, mutex->owner, mutex->owner->name, os_task_current(), os_task_current()->name),
        return (
          log_warn("os_mutex_lock: waiter list overflow for mutex %p (owner %p '%s'), lock for %p '%s' failed",
            mutex, mutex->owner, mutex->owner->name, os_task_current(), os_task_current()->name),
          false
        ));
    }

    if (timeout) {
      OS_LOG_TRACE(MUTEX, "os_mutex_lock: task '%s' WAITING (% dms) on %p",
        os_task_current()->name, timeout->duration, mutex);

      // If lock timeout is specified, delay for specified time,
      // after which os_mutex_try_lock will get called
      os_delay(timeout->duration);
    } else {
      OS_LOG_TRACE(MUTEX, "os_mutex_lock: task '%s' LOCKED on %p",
        os_task_current()->name, mutex);

      // If timeout == NULL, lock task on mutex indefinitely,
      // until mutex is unlocked
      os_task_current()->state = OS_TASK_STATE_LOCKED;
      os_schedule();

      // When scheduler switches to current task after indefinite lock
      // retry whole process again, because if we were to try_lock and
      // fail, it will defeat the purpose of indefinite lock
      return os_mutex_lock(mutex, NULL);
    }
  }

  // Execution reaches here if mutex is locked and timeout has expired, or
  // mutex wal unlocked from the beginning
  return os_mutex_try_lock(mutex);
}

bool os_mutex_try_lock(os_mutex_t * mutex) {
  ASSERT_RETURN(mutex, false);

  OS_LOG_TRACE(MUTEX, "os_mutex_try_lock: %p (owner %p '%s') by %p '%s'",
    mutex, mutex->owner, mutex->owner->name,
    os_task_current(), os_task_current()->name);

  if (mutex->status == OS_MUTEX_UNLOCKED) {
    // Set mutex state to locked
    mutex->status = OS_MUTEX_LOCKED;

    // Transfer ownership to curren task
    mutex->owner = os_task_current();

    // Clear current task from waiters
    // TODO: Maybe notify all and clear all?
    for (uint8_t i = 0; i < OS_MUTEX_MAX_WAITERS; ++i) {
      if (mutex->waiters[i] == os_task_current()) {
        mutex->waiters[i] = NULL;
      }
    }

    OS_LOG_TRACE(MUTEX, "os_mutex_try_lock: %p locked by %p '%s'",
      mutex, os_task_current(), os_task_current()->name);

    // Return true signalling that lock was successful
    return true;
  }

  OS_LOG_TRACE(MUTEX, "os_mutex_try_lock: %p can't lock (task %p '%s' tried)",
      mutex, os_task_current(), os_task_current()->name);

  // If mutex is locked, don't wait, just return false, signalling that
  // lock has failed
  return false;
}

void os_mutex_unlock(os_mutex_t * mutex) {
  ASSERT_RETURN(mutex);

  OS_LOG_TRACE(MUTEX, "os_mutex_unlock: %p (owner %p '%s') by %p '%s'",
    mutex, mutex->owner, mutex->owner->name,
    os_task_current(), os_task_current()->name);

  if (mutex->status == OS_MUTEX_UNLOCKED) {
    OS_LOG_TRACE(MUTEX, "os_mutex_unlock: %p already unlocked (task %p '%s' tried)",
      mutex, os_task_current(), os_task_current()->name);
    // If mutex is already unlocked - do nothing
    return;
  }

  // Notify all valid waiters
  for (uint8_t i = 0; i < OS_MUTEX_MAX_WAITERS; ++i) {
    if (mutex->waiters[i]) {
      OS_LOG_TRACE(MUTEX, "os_mutex_unlock: notify %p '%s' on %p unlock",
        mutex->waiters[i], mutex->waiters[i]->name, mutex);

      // For each waiter, set it's state to WAITING and timeout to
      // corresponding waiter index, to preserve lock order
      mutex->waiters[i]->state = OS_TASK_STATE_WAITING;
      timeout_start(&mutex->waiters[i]->wait_timeout, i);
      mutex->waiters[i] = NULL;
    }
  }

  // Set mutex state to unlocked
  mutex->status = OS_MUTEX_UNLOCKED;

  OS_LOG_TRACE(MUTEX, "os_mutex_unlock: %p unlocked by %p '%s'",
      mutex, os_task_current(), os_task_current()->name);
}
