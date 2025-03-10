/** ========================================================================= *
 *
 * @file trace_alloc.h
 * @date 10-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Allocator trace. Can trace calls to alloc/free and detect leaks
 *
 * TODO: Maybe use gcc wrappers for target alloc/free?
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdbool.h>
#include <stddef.h>

/**
 * Include header needed by TRACE_ALLOC_PORT_ALLOC_FN/TRACE_ALLOC_PORT_FREE_FN
 * if exists
 */
#ifdef TRACE_ALLOC_PORT_ALLOC_INC
#include TRACE_ALLOC_PORT_ALLOC_INC
#endif

/**
 * Include header needed by TRACE_ALLOC_PORT_LOG if exists
 */
#ifdef TRACE_ALLOC_PORT_LOG_INC
#include TRACE_ALLOC_PORT_LOG_INC
#endif

/* Defines ================================================================== */
/**
 * Max allocations that can be traced
 */
#ifndef TRACE_ALLOC_BUF_SIZE
#define TRACE_ALLOC_BUF_SIZE              32
#endif

/**
 * If enabled, trace_alloc context global instance will be used, if NULL is
 * passed to trace_alloc_start/trace_alloc_end
 */
#ifndef TRACE_ALLOC_USE_SINGLE_INSTANCE
#define TRACE_ALLOC_USE_SINGLE_INSTANCE   1
#endif

/**
 * Traced alloc function
 */
#ifndef TRACE_ALLOC_PORT_ALLOC_FN
#define TRACE_ALLOC_PORT_ALLOC_FN         os_alloc
#endif

/**
 * Traced free function
 */
#ifndef TRACE_ALLOC_PORT_FREE_FN
#define TRACE_ALLOC_PORT_FREE_FN          os_free
#endif

/**
 * Log print function
 */
#ifndef TRACE_ALLOC_PORT_LOG
#define TRACE_ALLOC_PORT_LOG              log_printf
#endif

/* Macros =================================================================== */
/**
 * Calls TRACE_ALLOC_PORT_ALLOC_FN and saves allocation context
 * Wraps call to trace_alloc_impl to save call location
 *
 * @param size
 */
#define trace_alloc(size)                                                       \
  trace_alloc_impl(size, __FUNCTION__, __LINE__)

/**
 * Calls TRACE_ALLOC_PORT_FREE_FN and saves free context
 * Wraps call to trace_free_impl to save call location
 *
 * @param size
 */
#define trace_free(ptr)                                                         \
  trace_free_impl(ptr, __FUNCTION__, __LINE__)

/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Single allocation context
 */
typedef struct {
  /**
   * Allocation info
   */
  struct {
    void * ptr;
    size_t size;
  } info;

  /**
   * trace_alloc() call location
   */
  struct {
    const char * fn;
    size_t       line;
  } location;

  /**
   * Needed for trace_alloc_ctx_t
   */
  bool allocated;
} trace_alloc_t;

/**
 * Context for trace_alloc/trace_free to save allocation traces to
 */
typedef struct {
  trace_alloc_t allocations[TRACE_ALLOC_BUF_SIZE];
  bool enabled;
  bool trace_only;
} trace_alloc_ctx_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Starts trace
 *
 * @param ctx Trace context. If TRACE_ALLOC_USE_SINGLE_INSTANCE is enabled and
 *            ctx is NULL, will use global instance
 */
void trace_alloc_start(trace_alloc_ctx_t * ctx);

/**
 * Stops trace
 *
 * @param ctx Trace context. If TRACE_ALLOC_USE_SINGLE_INSTANCE is enabled and
 *            ctx is NULL, will use global instance
 * @return Number of leaks, if any
 */
size_t trace_alloc_end(trace_alloc_ctx_t * ctx);

/**
 * Implementation for trace_alloc, not meant to be called directly
 *
 * @param size Allocation size
 * @param fn Function name that called trace_alloc
 * @param line Line in file that called trace_alloc
 * @return Allocated by TRACE_ALLOC_PORT_ALLOC_FN memory
 */
void * trace_alloc_impl(size_t size, const char * fn, size_t line);

/**
 * Implementation for trace_free, not meant to be called directly
 *
 * @param ptr Allocated by trace_alloc block of memory
 * @param fn Function name that called trace_free
 * @param line Line in file that called trace_free
 */
void trace_free_impl(void * ptr, const char * fn, size_t line);

/**
 * Returns current instance of trace_alloc_ctx_t
 *
 * @bite If TRACE_ALLOC_USE_SINGLE_INSTANCE is enabled will be implemented and
 *       will return global instance, if disabled, user must implement this
 *
 * @return trace_alloc context
 */
trace_alloc_ctx_t * trace_alloc_get_instance(void);

#ifdef __cplusplus
}
#endif