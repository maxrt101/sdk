/** ========================================================================= *
*
 * @file os.h
 * @date 27-02-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * TODO: Add user task context?
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
 * Sleep after OS_SLEEP_AFTER_CYCLES cycles passed
 */
#ifndef USE_OS_SLEEP_AFTER_CYCLE
#define USE_OS_SLEEP_AFTER_CYCLE              0
#endif

/**
 * Cycles to trigger sleep after
 * (enabled with USE_OS_SLEEP_AFTER_CYCLE)
 */
#ifndef OS_SLEEP_AFTER_CYCLES
#define OS_SLEEP_AFTER_CYCLES                 0
#endif

/**
 * Sleep mode to trigger after OS_SLEEP_AFTER_CYCLES
 * (enabled with USE_OS_SLEEP_AFTER_CYCLE)
 */
#ifndef OS_SLEEP_MODE
#define OS_SLEEP_MODE                         OS_POWER_MODE_FAST_SLEEP
#endif

/**
 * If enabled - will call wdt_feed on each cycle
 */
#ifndef OS_WDT_AUTOFEED
#define OS_WDT_AUTOFEED                       1
#endif

/**
 * If enabled - will call soft_wdt_check on each cycle
 */
#ifndef OS_USE_SOFT_WDT
#define OS_USE_SOFT_WDT                       0
#endif

/**
 * If enabled - will enable os_stat
 */
#ifndef USE_OS_STAT
#define USE_OS_STAT                           1
#endif

/**
 * If enabled - will trance stack usage for each task
 */
#ifndef OS_STAT_TRACE_TASK_STACK
#define OS_STAT_TRACE_TASK_STACK              1
#endif

/**
 * Period in cycles, how often check stack usage
 */
#ifndef OS_STAT_TRACE_TASK_STACK_CYCLES
#define OS_STAT_TRACE_TASK_STACK_CYCLES            10
#endif


/**
 * If enabled, will log every scheduler cycle number and tick
 */
#ifndef USE_OS_TRACE_CYCLE
#define USE_OS_TRACE_CYCLE                    0
#endif

/**
 * If enabled, will log calls to os_task_kill
 */
#ifndef USE_OS_TRACE_TASK_KILL
#define USE_OS_TRACE_TASK_KILL                0
#endif

/**
 * If enabled, will log calls to os_yield
 */
#ifndef USE_OS_TRACE_TASK_YIELD
#define USE_OS_TRACE_TASK_YIELD               0
#endif

/**
 * If enabled, will log task switching
 */
#ifndef USE_OS_TRACE_TASK_SWITCH
#define USE_OS_TRACE_TASK_SWITCH              0
#endif

/**
 * If enabled, will log every handled task for each cycle
 */
#ifndef USE_OS_TRACE_TASK_HANDLE
#define USE_OS_TRACE_TASK_HANDLE              0
#endif

/**
 * Enables setjmp/longjmp trace
 * Will print jmp_buf address and function:line for each invocation of
 * setjmp/longjmp
 */
#ifndef USE_OS_TRACE_SETJMP
#define USE_OS_TRACE_SETJMP                   0
#endif

/* Macros =================================================================== */
/**
 * Get (previously declared via OS_DECLARE_TASK) task handle from name
 *
 * @param __name Task name
 */
#define OS_TASK(__name) &UTIL_CAT(__name, _task)

/**
 * Declares (but not defines) a task handle, useful when a static task needs
 * to be accessed
 *
 * @param __name Task name
 */
#define OS_DECLARE_TASK(__name)                                          \
  extern os_task_t UTIL_CAT(__name, _task)

/**
 * Creates task stack and handles, initializes handle
 *
 * @param __name        Task name. A string
 * @param __stack_size  Size of task stack in bytes
 * @param __fn          Task function
 * @param __arg         Task function argument. void pointer
 * @param ...           Task priority (bigger - higher, default 0)
 */
#define OS_CREATE_TASK(__name, __stack_size, __fn, __arg, ...)          \
  uint8_t UTIL_CAT(__name, _task_stack)[__stack_size] __ALIGNED(8);     \
  os_task_t UTIL_CAT(__name, _task) = {                                 \
    .next = NULL,                                                       \
    .state = OS_TASK_STATE_NONE,                                        \
    .priority = UTIL_IF_EMPTY(__VA_ARGS__, 0, __VA_ARGS__),             \
    .name = UTIL_STRINGIFY(__name),                                     \
    .stack.start = UTIL_CAT(__name, _task_stack),                       \
    .stack.end = UTIL_CAT(__name, _task_stack) + __stack_size,          \
    .fn = __fn,                                                         \
    .arg = __arg,                                                       \
  }

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

/**
 * Signal types
 */
typedef enum {
  OS_SIGNAL_NONE          = 0,
  OS_SIGNAL_USER          = (1 << 0),
  OS_SIGNAL_PAUSE         = (1 << 1),
  OS_SIGNAL_RESUME        = (1 << 2),
  OS_SIGNAL_KILL          = (1 << 3),

  OS_SIGNAL_ALL           = 0xFF
} os_signal_t;

/* Types ==================================================================== */
/**
 * Task context - registers and whatnot, used by setjmp/longjmp
 */
typedef union {
  jmp_buf buf;
} os_task_ctx_t;

/**
 * Task function
 */
typedef void (*os_task_fn_t)(void *);

/**
 * Task signal handler function
 */
typedef void (*os_task_signal_handler_t)(os_signal_t signal, void *);

/**
 * Task context used by os
 */
typedef struct os_task_t {
  /** Task contexts are organized in a linked list */
  struct os_task_t *        next;

  os_task_state_t           state;
  uint8_t                   priority;
  const char *              name;

  os_task_ctx_t             ctx;

  /** Stores info about task stack */
  struct {
    void *                  start;
    void *                  end;

    /** Stores last known stack pointer, if enabled */
    UTIL_IF_1(OS_STAT_TRACE_TASK_STACK, void * last_sp);
  } stack;

  /** User argument to fn & sig */
  void *                    arg;

  /** Task worker */
  os_task_fn_t              fn;

  /** Signal handler */
  os_task_signal_handler_t  sig;

  /** Number of cycles, a task ran for */
  size_t                    cycles;

  /** Timeout for os_delay, while not expired, will skip returning to task context */
  timeout_t                 wait_timeout;

  /** Mask of os_signal_t values, which is used to decide whether to call sig handler */
  uint8_t                   signals;
} os_task_t;

/**
 * Task stats
 */
typedef struct {
  const char *    name;
  uint8_t         priority;
  size_t          stack_size;
  size_t          stack_used;
  size_t          cycles;
  os_task_state_t state;
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
 * Send signal to a task (will call signal handler)
 *
 * Signals themselves don't do anything, they are meant to signal tasks
 * about pending changes
 *
 * @param task Task handle
 * @param signal Signal type
 */
void os_signal(os_task_t * task, os_signal_t signal);

/**
 * Register signal handler for current task
 *
 * @note if signals_mask needs to be changed, but handler function does
 * not - pass NULL as fn
 *
 * @param signals_mask Bitmask of signal types to subscribe to
 * @param fn Handler function
 */
void os_signal_register_handler(uint8_t signals_mask, os_task_signal_handler_t fn);

/**
 * Returns true if task is not running (not initialized, paused or exited)
 *
 * @param task Task to check
 * @return true if running, otherwise - false
 */
bool os_task_is_running(os_task_t * task);

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
 * Sets task priority
 *
 * @param task Task handle
 */
void os_task_set_priority(os_task_t * task, uint8_t priority);

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
 * Iterate through tasks
 *
 * Example:
 * @code{.c}
 * os_task_t * iter = NULL;
 *
 * while (os_task_iter(&iter)) {
 *   ...
 * }
 * @endcode
 *
 * @param task pointer to temporary task buffer
 * @retval true if next task exists, false if not
 */
bool os_task_iter(os_task_t ** task);

/**
 * Retrieve task statistics
 *
 * @param[in] task Task handle
 * @param[out] stat Task stats
 */
void os_task_stat(os_task_t * task, os_task_stat_t * stat);

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