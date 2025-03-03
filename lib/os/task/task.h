/** ========================================================================= *
 *
 * @file task.h
 * @date 16-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Task support
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/util.h"
#include <stdint.h>
#include <stddef.h>

/* Defines ================================================================== */
/**
 * Default stack size in 32 bit integers (e.g. 256 x 4 (sizeof uint32_t))
 */
#define OS_DEFAULT_STACK_SIZE 256

/* Macros =================================================================== */
/**
 * Return custom stack size if provided, or OS_DEFAULT_STACK_SIZE
 */
#define OS_TASK_STACK_SIZE_IMPL(...) \
    UTIL_IF_EMPTY(__VA_ARGS__, OS_DEFAULT_STACK_SIZE, __VA_ARGS__)

/**
 * Defines a struct with task context and task stack
 *
 * @param name Task name
 * @param ...  Optional task stack size in 32 bit integers
 *             Default value - OS_DEFAULT_STACK_SIZE
 */
#define OS_TASK_DEFINE(name, ...)                                     \
    struct {                                                          \
      struct {                                                        \
        uint32_t size;                                                \
        uint32_t buffer[OS_TASK_STACK_SIZE_IMPL(__VA_ARGS__)];        \
      } stack;                                                        \
      os_task_t task;                                                 \
    } UTIL_CAT(name, _task) = {OS_TASK_STACK_SIZE_IMPL(__VA_ARGS__)};

/**
 * Calls os_create_task
 * Automatically derives task, stack & stack_size from task name
 *
 * @note Task context must be defined using OS_TASK_DEFINE
 *
 * @param name Task name
 * @param fn   Task worker function
 * @param ctx  Task worker context
 */
#define OS_TASK_CREATE(name, fn, ctx)     \
    os_create_task(                       \
      &UTIL_CAT(name, _task).task,        \
      fn, ctx,                            \
      UTIL_CAT(name, _task).stack.buffer, \
      UTIL_CAT(name, _task).stack.size    \
    )

/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Task callback declaration
 */
typedef void (*os_task_handler_t)(void *);

/**
 * Task context
 */
typedef struct os_task_s {
  /**
   * Current value of stack pointer register
   */
  uint32_t * sp;

  /**
   * Pointer to next task (NULL if at the end of the task list)
   */
  struct os_task_s * next;

  /**
   * Function pointer to task callback (worker)
   */
  os_task_handler_t fn;

  /**
   * Context of the callback
   */
  void * ctx;

  /**
   * Pointer to start of tasks stack
   */
  uint32_t * stack;

  /**
   * Size of tasks stack
   */
  size_t stack_size;
} os_task_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Creates a task (initializes the handle and notifies scheduler about new task)
 *
 * @note 'task' must be a pointer to global variable (or any retain-able piece
 *       of RAM for that matter)
 *
 * @param task Task handle to initialize (Can't be a local variable!)
 * @param fn Task callback (worker)
 * @param ctx Context of the callback
 * @param stack Pointer to bottom of the stack
 * @param stack_size Size of the stack
 */
void os_create_task(os_task_t * task, os_task_handler_t fn, void * ctx,
                    uint32_t * stack, size_t stack_size);

/**
 * Returns task size in bytes
 *
 * @param task Task handle
 */
size_t os_get_stack_size(os_task_t * task);

/**
 * Returns pointer to stack top
 *
 * @param task Task handle
 */
uint8_t * os_get_stack_top(os_task_t * task);

#ifdef __cplusplus
}
#endif