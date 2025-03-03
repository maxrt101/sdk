/** ========================================================================= *
 *
 * @file task.c
 * @date 16-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Task support
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "os/task/task.h"
#include "os/task/scheduler.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG TASK

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
extern volatile os_task_t * os_task_current;
extern volatile os_task_t * os_task_head;

/* Private functions ======================================================== */
/* Shared functions ========================================================= */
void os_create_task(os_task_t * task, os_task_handler_t fn, void * ctx,
                    uint32_t * stack, size_t stack_size) {
  // TODO: If spawning tasks at runtime is feasible, wrap this whole
  //       initialization in an atomic block
  task->fn = fn;
  task->ctx = ctx;
  task->stack = stack;
  task->stack_size = stack_size;

  if (!os_task_head) {
    os_task_head = task;
    os_task_current = os_task_head;
  } else {
    volatile os_task_t * tmp = os_task_head;
    while (tmp->next) {
      tmp = tmp->next;
    }
    tmp->next = task;
  }

  task->sp = &stack[stack_size-16];    // Allocate 1 frame for initial task start
  stack[stack_size-1] = 0x01000000;    // Set thumb bit in PSR
  stack[stack_size-2] = (uint32_t) fn; // Set pc to worker function
  stack[stack_size-3] = (uint32_t) os_task_exited; // Set LR to exit callback
  stack[stack_size-8] = (uint32_t) ctx; // Set R0 to ctx parameter

  task->next = NULL;

  log_debug("os_create_task: task=%p fn=%p stack=%p size=%u sp=%p",
            task, fn, stack, stack_size, task->sp);
}

size_t os_get_stack_size(os_task_t * task) {
  ASSERT_RETURN(task, 0);
  return task->stack_size * sizeof(uint32_t);
}

uint8_t * os_get_stack_top(os_task_t * task) {
  ASSERT_RETURN(task, NULL);
  return (uint8_t*)task->stack + os_get_stack_size(task);
}
