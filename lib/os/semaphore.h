/** ========================================================================= *
 *
 * @file semaphore.h
 * @date 19-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Generic semaphore implementation
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdint.h>
#include "error/error.h"
#include "error/assertion.h"
#include "time/timeout.h"
#include "util/compiler.h"
#include "atomic/atomic.h"

/* Defines ================================================================== */
/**
 * Wait forever on semaphore acquire
 */
#define OS_SEM_WAIT_FOREVER (-1UL)

/* Macros =================================================================== */
/**
 * Can be put before a block to make a block of code acquire semaphore on
 * enter and release on exit
 */
#define OS_SEM_BLOCK(sem, ms)                           \
  for (bool e = (os_semaphore_acquire(sem, ms), true);  \
      e; e = (os_semaphore_release(sem), false))

/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Generic semaphore context
 */
typedef struct {
  uint8_t value;
  uint8_t max;
} os_semaphore_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes semaphore
 *
 * @param sem Semaphore handle
 * @param init_val Initial value for semaphore
 * @param max_val Maximal value for semaphore
 */
__STATIC_INLINE error_t os_semaphore_init(os_semaphore_t * sem, uint8_t init_val, uint8_t max_val) {
  ASSERT_RETURN(sem, E_NULL);

  sem->value = init_val;
  sem->max = max_val;

  return E_OK;
}

/**
 * Releases semaphore
 *
 * @param sem Semaphore handle
 */
__STATIC_INLINE error_t os_semaphore_release(os_semaphore_t * sem) {
  ASSERT_RETURN(sem, E_NULL);

  ATOMIC_BLOCK() {
    if (sem->value < sem->max) {
      sem->value++;
    }
  }

  return E_OK;
}

/**
 * Acquires semaphore
 *
 * @note pass OS_SEM_WAIT_FOREVER as timeout_ms to wait forever
 *
 * @param sem Semaphore handle
 * @param timeout_ms Timeout in ms, if expires and semaphore can't be acquired,
 *                   the function will return E_TIMEOUT
 */
__STATIC_INLINE error_t os_semaphore_acquire(os_semaphore_t * sem, milliseconds_t timeout_ms) {
  ASSERT_RETURN(sem, E_NULL);

  timeout_t t;

  if (timeout_ms != OS_SEM_WAIT_FOREVER) {
    timeout_start(&t, timeout_ms);
  }

  while (!sem->value) {
    if (timeout_ms != OS_SEM_WAIT_FOREVER && timeout_is_expired(&t)) {
      return E_TIMEOUT;
    }
  }

  ATOMIC_BLOCK() {
    --sem->value;
  }
  return E_OK;
}

/**
 * Acquires semaphore, if available, return E_BUSY otherwise
 *
 * @param sem Semaphore handle
 */
__STATIC_INLINE error_t os_semaphore_try_acquire(os_semaphore_t * sem) {
  ASSERT_RETURN(sem, E_NULL);

  error_t err = os_semaphore_acquire(sem, 0);

  return err == E_TIMEOUT ? E_BUSY : err;
}

#ifdef __cplusplus
}
#endif