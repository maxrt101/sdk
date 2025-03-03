/** ========================================================================= *
 *
 * @file atomic.h
 * @date 19-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Library for atomic code execution
 *
 * @note Since this is bare-metal environment without operating system or
 *       any kind of parallel execution, the only thing that can prevent
 *       code from executing linearly is interrupts, so we just disable them
 *       for the duration of atomic block
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/compiler.h"
#include "os/irq/irq.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Macro for atomic execution inside a block
 *
 * @example ATOMIC_BLOCK() { ... some code ... }
 */
#define ATOMIC_BLOCK(...)                         \
    for (atomic_block_state_t s = atomic_begin(); \
         s != ATOMIC_BLOCK_STATE_EXIT;            \
         s = atomic_end())

/* Enums ==================================================================== */
typedef enum {
  ATOMIC_BLOCK_STATE_NONE = 0,
  ATOMIC_BLOCK_STATE_ENTER,
  ATOMIC_BLOCK_STATE_EXIT,
} atomic_block_state_t;

/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Enter atomic execution block
 */
__STATIC_INLINE atomic_block_state_t atomic_begin(void) {
  os_irq_disable(OS_IRQ_ALL);
  return ATOMIC_BLOCK_STATE_ENTER;
}

/**
 * Exit atomic execution block
 */
__STATIC_INLINE atomic_block_state_t atomic_end(void) {
  os_irq_enable(OS_IRQ_ALL);
  return ATOMIC_BLOCK_STATE_EXIT;
}

#ifdef __cplusplus
}
#endif