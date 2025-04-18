/** ========================================================================= *
*
 * @file os.c
 * @date 27-02-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "log/log.h"
#include "error/assertion.h"
#include "os/abort/abort.h"
#include "os/power/power.h"
#include "os.h"

#if OS_WDT_AUTOFEED
#include "wdt/wdt.h"
#endif

#if OS_USE_SOFT_WDT
#include "swdg/soft_wdt.h"
#endif

/* Defines ================================================================== */
#define LOG_TAG OS

/**
 * Can statically limit number of cycles that the scheduler will run for
 * After exceeding MAX_CYCLES aborts
 */
#define USE_MAX_CYCLES                    0

/**
 * Static limit to how many cycles will the scheduler run for
 * Enabled by USE_MAX_CYCLES
 */
#define MAX_CYCLES                        20

/**
 * Delay for CYCLE_DELAY ms time on each cycle
 */
#define USE_CYCLE_DELAY                   0

/**
 * Time in ms to delay each cycle for
 */
#define CYCLE_DELAY                       200

/**
 * __setjmp can be redefined, if needed to provide custom implementation
 * if not defined - will use libc's implementation
 *
 * @note Works only with USE_OS_TRACE_SETJMP
 */
#ifndef __setjmp
#define __setjmp setjmp
#endif

/**
 * __longjmp can be redefined, if needed to provide custom implementation
 * if not defined - will use libc's implementation
 *
 * @note Works only with USE_OS_TRACE_SETJMP
 */
#ifndef __longjmp
#define __longjmp longjmp
#endif

/* Macros =================================================================== */
#if USE_OS_TRACE_SETJMP
/**
 * If USE_OS_TRACE_SETJMP is enabled, replaces setjmp with this macro
 * to trace setjmp calls.
 *
 * Calls log_debug, DUMP_JMP_BUF (if USE_OS_TRACE_SETJMP_DUMP_JMP_BUF is
 * enabled) and finally original setjmp
 *
 * @param __buf jmp_buf to be passed to setjmp
 */
#define setjmp(__buf)                                                     \
  (                                                                       \
    log_debug("setjmp(%p)  at %s:%d", &__buf, __FUNCTION__, __LINE__),    \
    __setjmp(__buf)                                                       \
  )

/**
 * If USE_OS_TRACE_SETJMP is enabled, replaces longjmp with this macro
 * to trace longjmp calls.
 *
 * Calls log_debug, DUMP_JMP_BUF (if USE_OS_TRACE_SETJMP_DUMP_JMP_BUF is
 * enabled) and finally original longjmp
 *
 * @param __buf jmp_buf to be passed to longjmp
 */
#define longjmp(__buf, __ret)                                             \
  (                                                                       \
    log_debug("longjmp(%p) at %s:%d", &__buf, __FUNCTION__, __LINE__),    \
    __longjmp(__buf, __ret)                                               \
  )
#endif

/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * OS Context
 */
typedef struct {
  os_task_ctx_t ctx;

  struct {
    os_task_t * head;
    os_task_t * current;
  } task;

  uint32_t cycles;
} os_t;

/* Variables ================================================================ */
/**
 * Global OS Context
 */
static os_t os;

/* Private functions ======================================================== */
/**
 * Initializes stack for task
 *
 * Calls os_set_stack_port to setup stack and fills it with magic to detect
 * stack overflows if USE_OS_STACK_CHECK is enabled
 *
 * @param task Task handle
 */
__STATIC_INLINE void os_task_stack_init(os_task_t * task) {
#if USE_OS_STACK_CHECK
  for (uint32_t * sp = task->stack.start; sp < (uint32_t *) task->stack.end; sp++) {
    *sp = OS_STACK_MAGIC;
  }
#endif

  os_set_stack_port(os.task.current->stack.end);
}

/**
 * Advances os.task.current
 */
__STATIC_INLINE void os_task_next(void) {
  os.task.current = os.task.current->next;

  if (!os.task.current) {
    os.task.current = os.task.head;
  }
}

/* Shared functions ========================================================= */
void os_task_start(os_task_t * task) {
  ASSERT_RETURN(task);

  if (!os.task.head) {
    os.task.head = task;
    os.task.current = os.task.head;
  } else {
    volatile os_task_t * tmp = os.task.head;
    while (tmp->next) {
      tmp = tmp->next;
    }
    tmp->next = task;
  }

  task->next = NULL;

  task->state = OS_TASK_STATE_INIT;

  log_info("os_task_start(%p): name='%s' stack=(%p %p)", task, task->name, task->stack.start, task->stack.end);
}

void os_task_create(
  os_task_t * task,
  const char * name,
  uint8_t * stack,
  size_t stack_size,
  os_task_fn_t fn,
  void * arg
) {
  ASSERT_RETURN(task && name && stack && stack_size && fn);

  memset(task, 0, sizeof(*task));

  task->state = OS_TASK_STATE_NONE;
  task->name = name;
  task->stack.start = stack;
  task->stack.end = stack + stack_size;
  task->fn = fn;
  task->arg = arg;

  os_task_start(task);
}

// __OPTIMIZE(0)
void os_launch(void) {
  log_info("Init scheduler");

  os.cycles = 0;

  // Prepares stack for scheduler
  os_prepare_scheduler_stack_port();

  // Set current task to head of task list
  os.task.current = os.task.head;

  // This is the main scheduler loop, everything happens here
  while (1) {
    // Increase cycle counter
    os.cycles++;

    OS_LOG_TRACE(CYCLE, "Cycle %d (tick=%d)", os.cycles, runtime_get());

#if USE_MAX_CYCLES
    if (os.cycles == MAX_CYCLES) {
      os_abort("debug max cycles reached");
    }
#endif

#if USE_OS_SLEEP_AFTER_CYCLE
    if (os.cycles && os.cycles % OS_SLEEP_AFTER_CYCLES == 0) {
      os_power_mode_change(OS_SLEEP_MODE);
    }
#endif

    // Initialize task, if it is not
    if (os.task.current->state == OS_TASK_STATE_INIT) {
      log_info("Init task %p '%s'", os.task.current, os.task.current->name);
      os.task.current->state = OS_TASK_STATE_READY;

      // Next call to os_schedule will return here
      if (setjmp(os.ctx.buf)) {
        os_task_next();
        continue;
      }

      // Setup stack for task
      os_task_stack_init(os.task.current);

      // Call task function
      // Upon next os_schedule it will return to setjmp we did earlier
      os.task.current->fn(os.task.current->arg);

      // Only reached if task function executes a `return`
      log_warn("Task '%s': function returned", os.task.current->name);

      // If enabled - abort if task returns, otherwise just call os_exit
      UTIL_IF_1(OS_ABORT_ON_TASK_EXIT,
        os_abort("Task %p '%s' returned", os.task.current, os.task.current->name),
        os_exit());
    }

    // Sleep for specified time, if cycle delay is enabled
    // sleep_ms doesn't rely on OS or timeouts or even timers
    UTIL_IF_1(USE_CYCLE_DELAY, sleep_ms(CYCLE_DELAY));

    // Handle next task in list
    OS_LOG_TRACE(TASK_HANDLE, "Task %p '%s' (%s)",
      os.task.current, os.task.current->name, os_task_state_to_str(os.task.current->state));

    // If task is waiting on a timeout, and that timeout is expired - make in ready again
    if (os.task.current->state == OS_TASK_STATE_WAITING && timeout_is_expired(&os.task.current->wait_timeout)) {
      os.task.current->state = OS_TASK_STATE_READY;
    }

    UTIL_IF_1(OS_WDT_AUTOFEED, wdt_feed());

    // If task is ready - switch to it
    if (os.task.current->state == OS_TASK_STATE_READY) {
      OS_LOG_TRACE(TASK_SWITCH, "Task %p '%s' ready, switching now",
        os.task.current, os.task.current->name);

      // Upon next os_schedule call - execution will advance to os_task_next() and next cycle will begin
      if (!setjmp(os.ctx.buf)) {
        longjmp(os.task.current->ctx.buf, 1);
      }
    }

    UTIL_IF_1(OS_USE_SOFT_WDT, soft_wdt_check());

    // Advance current task to the next in the task list
    os_task_next();
  }

  // Technically unreachable
  // TODO: ???
  os_abort("");
}

void os_schedule(void) {
  OS_LOG_TRACE(TASK_YIELD, "Task '%s' yielded (%s)",
    os.task.current->name, os_task_state_to_str(os.task.current->state));

#if USE_OS_STACK_CHECK
  // Check stask overflow
  if (*((uint32_t *) os.task.current->stack.start) != OS_STACK_MAGIC) {
    os_abort("Stack overflow (task %p '%s')", os.task.current, os.task.current->name);
  }
#endif

  // Save current task context
  // Upon next task switch to this task, os_schedule will return, and execution
  // will resume where it left off
  if (setjmp(os.task.current->ctx.buf)) {
    return;
  }

  // Jump to scheduler
  longjmp(os.ctx.buf, 1);
}

void os_exit(void) {
  os.task.current->state = OS_TASK_STATE_EXITED;

  os_signal(os.task.current, OS_SIGNAL_KILL);

  os_task_t * tmp = os.task.head;

  while (tmp) {
    // Find current task in scheduler task list
    if (tmp->next == os.task.current) {
      // Remove current task from the list
      tmp->next = os.task.current->next;

      // Set task state to EXITED
      os.task.current->state = OS_TASK_STATE_EXITED;

      OS_LOG_TRACE(TASK_KILL, "Task %p '%s' exited",
        os.task.current, os.task.current->name);

      break;
    }
  }

  // Instead of calling os_schedule, manually setjmp task context to this point
  // so if task is somehow resumed - it won't actually run
  if (setjmp(os.task.current->ctx.buf)) {
    log_warn("Task '%s' exited, can't resume");
  }

  // Manually jump to scheduler
  longjmp(os.ctx.buf, 1);
}

void os_task_kill(os_task_t * task) {
  ASSERT_RETURN(task);

  // If task tries to do harakiri - abort or signal an error
  if (os.task.current == task) {
    UTIL_IF_1(OS_ABORT_ON_SELF_KILL,
      os_abort("Can't kill self - use os_exit()"),
      log_error("Can't kill self - use os_exit()"));
    return;
  }

  os_task_t * tmp = os.task.head;

  while (tmp) {
    // Find task in scheduler task list
    if (tmp->next == task) {
      // Remove task from task list
      tmp->next = task->next;

      // Set task state to EXITED
      task->state = OS_TASK_STATE_EXITED;

      os_signal(os.task.current, OS_SIGNAL_KILL);

      OS_LOG_TRACE(TASK_KILL, "Killed %p '%s'", task, task->name);

      return;
    }
  }

  // If task wasn't found in task list - abort or signal an error
  UTIL_IF_1(OS_ABORT_ON_KILL_NON_SCHEDULED_TASK,
    os_abort("Tried to kill not scheduled task %p '%s'", task, task->name),
    log_error("Tried to kill not scheduled task %p '%s'", task, task->name));
}

void os_delay(milliseconds_t ms) {
  // Start current task's wait timeout
  timeout_start(&os.task.current->wait_timeout, ms);

  // Scheduler will resume the task when timeout has expired
  os.task.current->state = OS_TASK_STATE_WAITING;

  // Return to scheduler
  os_schedule();
}

void os_task_pause(os_task_t * task) {
  ASSERT_RETURN(task);

  task->state = OS_TASK_STATE_PAUSED;

  os_signal(task, OS_SIGNAL_PAUSE);
}

void os_task_resume(os_task_t * task) {
  ASSERT_RETURN(task);

  task->state = OS_TASK_STATE_READY;

  os_signal(task, OS_SIGNAL_RESUME);
}

void os_signal(os_task_t * task, os_signal_t signal) {
  if (task->sig && task->signals & signal) {
    task->sig(signal, task->arg);
  }
}

void os_signal_register_handler(uint8_t signals_mask, os_task_signal_handler_t fn) {
  os.task.current->signals = signals_mask;
  if (fn) {
    os.task.current->sig = fn;
  }
}

bool os_task_is_running(os_task_t * task) {
  ASSERT_RETURN(task, false);

  return task->state != OS_TASK_STATE_NONE
      && task->state != OS_TASK_STATE_PAUSED
      && task->state != OS_TASK_STATE_EXITED;
}

os_task_t * os_task_get(const char * name) {
  os_task_t * tmp = os.task.head;

  while (tmp) {
    if (!strcmp(tmp->name, name)) {
      return tmp;
    }
    tmp = tmp->next;
  }

  return NULL;
}

os_task_t * os_task_current(void) {
  return os.task.current;
}

bool os_task_iter(os_task_t ** task) {
  ASSERT_RETURN(task, false);

  if (*task) {
    *task = (*task)->next;
    return *task;
  } else {
    *task = os.task.head;
    return true;
  }
}

const char * os_task_state_to_str(os_task_state_t state) {
  switch (state) {
    case OS_TASK_STATE_NONE:    return "NONE";
    case OS_TASK_STATE_INIT:    return "INIT";
    case OS_TASK_STATE_READY:   return "READY";
    case OS_TASK_STATE_PAUSED:  return "PAUSED";
    case OS_TASK_STATE_WAITING: return "WAITING";
    case OS_TASK_STATE_LOCKED:  return "LOCKED";
    case OS_TASK_STATE_EXITED:  return "EXITED";
    default:
      return "?";
  }
}

__WEAK void os_prepare_scheduler_stack_port(void) {
  os_abort("os_prepare_scheduler_stack_port has no implementation");
}

__WEAK void os_set_stack_port(void * stack) {
  os_abort("os_set_stack_port has no implementation");
}