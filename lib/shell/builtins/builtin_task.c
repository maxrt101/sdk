/** ========================================================================= *
*
 * @file builtin_task.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'task' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "shell/shell_util.h"
#include "log/log.h"
#include "os/os.h"
#include <string.h>

/* Defines ================================================================== */
#define LOG_TAG shell

/* Macros =================================================================== */
#define GET_TASK(name)                              \
    os_task_t * task = os_task_get(name);           \
    if (!task) {                                    \
        log_error("No task '%s'", name);            \
        return SHELL_FAIL;                          \
    }

/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
__STATIC_INLINE os_signal_t string_to_signal(const char * name) {
  if (!strcmp(name, "pause"))  return OS_SIGNAL_PAUSE;
  if (!strcmp(name, "resume")) return OS_SIGNAL_RESUME;
  if (!strcmp(name, "kill"))   return OS_SIGNAL_KILL;
  if (!strcmp(name, "user"))   return OS_SIGNAL_USER;

  return OS_SIGNAL_NONE;

}

/* Shared functions ========================================================= */
int8_t builtin_task(shell_t * sh, uint8_t argc, const char ** argv) {
  if (
      ( strcmp(argv[1], "list")   && argc < 2) ||
      (!strcmp(argv[1], "prio")   && argc < 3) ||
      (!strcmp(argv[1], "signal") && argc < 3)
  ) {
    log_error("Usage: task list|pause|resume|kill|prio|signal [TASK] [SIGNAL|PRIO]");
    return SHELL_FAIL;
  }

  if (!strcmp(argv[1], "list")) {
    os_task_t * task = NULL;

    while (os_task_iter(&task)) {
      os_task_stat_t stat;
      os_task_stat(task, &stat);

#if OS_STAT_TRACE_TASK_STACK
      log_printf(
        "%p %-8s %-8s %02d %4d/%-4d %d\r\n",
        task, stat.name, os_task_state_to_str(stat.state),
        stat.priority, stat.stack_used, stat.stack_size, stat.cycles
      );
#else
      log_printf(
        "%p %-8s %-8s %02d %d\r\n",
        task, stat.name, os_task_state_to_str(stat.state), stat.priority, stat.cycles
      );
#endif
    }
  } else if (!strcmp(argv[1], "pause")) {
    GET_TASK(argv[2]);
    os_task_pause(task);
  } else if (!strcmp(argv[1], "resume")) {
    GET_TASK(argv[2]);
    os_task_resume(task);
  } else if (!strcmp(argv[1], "kill")) {
    GET_TASK(argv[2]);
    os_task_kill(task);
  } else if (!strcmp(argv[1], "prio")) {
    GET_TASK(argv[2]);
    os_task_set_priority(task, shell_parse_int(argv[3]));
  } else if (!strcmp(argv[1], "signal")) {
    GET_TASK(argv[2]);
    os_signal(task, string_to_signal(argv[3]));
  } else {
    log_error("Invalid command '%s'", argv[1]);
    return SHELL_FAIL;
  }

  return SHELL_OK;
}
