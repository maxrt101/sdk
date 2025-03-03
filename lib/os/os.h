/** ========================================================================= *
*
 * @file os.h
 * @date 27-02-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "util/compiler.h"
#include "util/util.h"
#include "error/error.h"
#include "time/timeout.h"
#include "time/time.h"

#include <stdint.h>
#include <setjmp.h>

/* Defines ================================================================== */
/**
 * If enabled will abort if task function returns, otherwise will call os_exit
 */
#ifndef OS_ABORT_ON_TASK_EXIT
#define OS_ABORT_ON_TASK_EXIT                 0
#endif

/**
 * If enabled will abort if task tries to kill itself, instead of calling
 * os_exit, otherwise logs an error
 */
#ifndef OS_ABORT_ON_SELF_KILL
#define OS_ABORT_ON_SELF_KILL                 0
#endif

/**
 * If enabled will abort if os_task_kill is called on task that is not
 * scheduled, otherwise logs an error
 */
#ifndef OS_ABORT_ON_KILL_NON_SCHEDULED_TASK
#define OS_ABORT_ON_KILL_NON_SCHEDULED_TASK   0
#endif

/**
 * Enables stack integrity check
 */
#ifndef USE_OS_STACK_CHECK
#define USE_OS_STACK_CHECK                    1
#endif

/**
 * Magic value for stack integrity check
 */
#ifndef OS_STACK_MAGIC
#define OS_STACK_MAGIC                        0xDEADBEEF
#endif

/**
 * If enabled, will log every scheduler cycle number and tick
 */
#ifndef USE_OS_TRACE_CYCLE
#define USE_OS_TRACE_CYCLE                    1
#endif

/**
 * If enabled, will log calls to os_task_kill
 */
#ifndef USE_OS_TRACE_TASK_KILL
#define USE_OS_TRACE_TASK_KILL                1
#endif

/**
 * If enabled, will log calls to os_yield
 */
#ifndef USE_OS_TRACE_TASK_YIELD
#define USE_OS_TRACE_TASK_YIELD               1
#endif

/**
 * If enabled, will log task switching
 */
#ifndef USE_OS_TRACE_TASK_SWITCH
#define USE_OS_TRACE_TASK_SWITCH              1
#endif

/**
 * If enabled, will log every handled task for each cycle
 */
#ifndef USE_OS_TRACE_TASK_HANDLE
#define USE_OS_TRACE_TASK_HANDLE              1
#endif

/**
 * Enables setjmp/longjmp trace
 * Will print jmp_buf address and function:line for each invocation of
 * setjmp/longjmp
 */
#ifndef USE_OS_TRACE_SETJMP
#define USE_OS_TRACE_SETJMP                   0
#endif

/**
 * If USE_OS_TRACE_SETJMP is enabled, it can also dump jmp_buf
 */
#ifndef USE_OS_TRACE_SETJMP_DUMP_JMP_BUF
#define USE_OS_TRACE_SETJMP_DUMP_JMP_BUF      0
#endif

/* Macros =================================================================== */
/**
 * Creates task stack and handles, initializes handle
 *
 * @param __name        Task name. A string
 * @param __stack_size  Size of task stack in bytes
 * @param __fn          Task function
 * @param __arg         Task function argument. void pointer
 */
#define OS_CREATE_TASK(__name, __stack_size, __fn, __arg)               \
  uint8_t UTIL_CAT(__name, _task_stack)[__stack_size] __ALIGNED(8);     \
  os_task_t UTIL_CAT(__name, _task) = {                                 \
    .next = NULL,                                                       \
    .state = OS_TASK_STATE_NONE,                                        \
    .name = UTIL_STRINGIFY(__name),                                     \
    .stack.start = UTIL_CAT(__name, _task_stack),                       \
    .stack.end = UTIL_CAT(__name, _task_stack) + __stack_size,          \
    .fn = __fn,                                                         \
    .arg = __arg,                                                       \
  };

/**
 * Used internally to trace OS events
 * Use USE_OS_TRACE_* defines to control which events to trace
 *
 * @param __trace_module Name of module/event group to trace
 * @param ...            Passed as-is to underlying log_debug
 */
#define OS_LOG_TRACE(__trace_module, ...) \
  UTIL_IF_1(UTIL_CAT(USE_OS_TRACE_, __trace_module), log_debug(__VA_ARGS__))

/* Enums ==================================================================== */
/**
 * Task states
 */
typedef enum {
  OS_TASK_STATE_NONE      = 0,
  OS_TASK_STATE_INIT      = 1,
  OS_TASK_STATE_READY     = 2,
  OS_TASK_STATE_PAUSED    = 3,
  OS_TASK_STATE_WAITING   = 4,
  OS_TASK_STATE_LOCKED    = 5,
  OS_TASK_STATE_EXITED    = 6,
} os_task_state_t;

/* Types ==================================================================== */
/**
 * setjmp/longjmp buffer description for arm
 * TODO: Move outside, maybe remove altogether and expect os port to provide
 *       DUMP_JMP_BUF definition instead
 */
typedef struct {
  // r4 - r10, fp, sp, lr
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t fp;
  uint32_t sp;
  uint32_t lr;

  // d8 - d15
#if __FPU_PRESENT
  uint32_t d8;
  uint32_t d9;
  uint32_t d10;
  uint32_t d11;
  uint32_t d12;
  uint32_t d13;
  uint32_t d14;
  uint32_t d15;
#endif
} os_task_ctx_arch_t;

/**
 * Task context - registers and whatnot, used by setjmp/longjmp
 */
typedef union {
  os_task_ctx_arch_t frame;
  jmp_buf buf;
} os_task_ctx_t;

/**
 * Task function
 */
typedef void (*os_task_fn_t)(void *);

/**
 * Task context used by os
 */
typedef struct os_task_t {
  struct os_task_t * next;

  os_task_state_t state;
  const char * name;

  os_task_ctx_t ctx;

  struct {
    void * start;
    void * end;
  } stack;

  os_task_fn_t fn;
  void * arg;

  timeout_t wait_timeout;
} os_task_t;

/**
 * Task stat
 *
 * TODO: Implement os_task_stat
 */
typedef struct {
  os_task_state_t state;
  size_t stack_used;
} os_task_stat_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Appends initialized task to os scheduler task list
 *
 * @param task Task context
 */
void os_task_start(os_task_t * task);

/**
 * Manually creates task
 *
 * @param task        Task handle
 * @param name        Task name
 * @param stack       Stack pointer buffer start
 * @param stack_size  Size of stack
 * @param fn          Task function
 * @param arg         Task function argument
 */
void os_task_create(
  os_task_t * task,
  const char * name,
  uint8_t * stack,
  size_t stack_size,
  os_task_fn_t fn,
  void * arg
);

/**
 * Starts OS scheduler
 */
void os_launch(void);

/**
 * Returns to scheduler from task context
 */
void os_schedule(void);

/**
 * Delay task for at least `ms` milliseconds
 *
 * @param ms Milliseconds to delay task for
 */
void os_delay(milliseconds_t ms);

/**
 * Exits from current task
 */
void os_exit(void);

/**
 * Kills task
 *
 * @note Can't be called on self - to kill current task, use os_exit
 *
 * @param task Task handle to kill
 */
void os_task_kill(os_task_t * task);

/**
 * Pauses task, can be resumed later with os_task_resume
 *
 * @note Won't work on current task
 *
 * @param task Task handle to pause
 */
void os_task_pause(os_task_t * task);

/**
 * Resumes task, paused by os_task_pause
 *
 * @note Won't work on current task
 *
 * @param task Task handle to resume
 */
void os_task_resume(os_task_t * task);

/**
 * Retrieves task by name from scheduler task list
 *
 * @param name Task name
 * @retval NULL If no such task exists
 */
os_task_t * os_task_get(const char * name);

/**
 * Return handle of current task
 */
os_task_t * os_task_current(void);

/**
 * Yields execution
 *
 * Calls delay with 1 ms timeout
 * Sets current task state to WAITING and returns to scheduler
 */
__STATIC_INLINE void os_yield(void) {
  os_delay(1);
}

/**
 * Converts os_task_state_t enum value to it's string representation
 *
 * @param state Task state to convert
 * @retval ? If state is invalid
 */
const char * os_task_state_to_str(os_task_state_t state);

/**
 * OS Port function that prepares stack for OS scheduler
 */
void os_prepare_scheduler_stack_port(void);

/**
 * OS Port function that prepares stack for task
 *
 * @param stack Top of task stack pointer
 */
void os_set_stack_port(void * stack);

#ifdef __cplusplus
}
#endif