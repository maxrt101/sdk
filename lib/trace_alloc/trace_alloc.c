/** ========================================================================= *
 *
 * @file trace_alloc.c
 * @date 10-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "trace_alloc/trace_alloc.h"
#include "os/alloc/alloc.h"
#include "tty/ansi.h"
#include "log/log.h"

/* Defines ================================================================== */
#define TRACE_ALLOC_LOC_FMT                                                     \
  ANSI_COLOR_FG_CYAN "%s" ANSI_TEXT_RESET ":" ANSI_COLOR_FG_MAGENTA "%zu" ANSI_TEXT_RESET

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
#if TRACE_ALLOC_USE_SINGLE_INSTANCE
static trace_alloc_ctx_t trace_alloc_instance;
#endif

/* Private functions ======================================================== */
/* Shared functions ========================================================= */
void trace_alloc_start(trace_alloc_ctx_t * ctx) {
#if TRACE_ALLOC_USE_SINGLE_INSTANCE
  if (!ctx) {
    ctx = &trace_alloc_instance;
  }
#endif

  memset(ctx, 0, sizeof(trace_alloc_ctx_t));

  ctx->enabled = true;
}

size_t trace_alloc_end(trace_alloc_ctx_t * ctx) {
#if TRACE_ALLOC_USE_SINGLE_INSTANCE
  if (!ctx) {
    ctx = &trace_alloc_instance;
  }
#endif

  ctx->enabled = false;

  size_t leaks = 0;

  for (size_t i = 0; i < TRACE_ALLOC_BUF_SIZE; ++i) {
    if (ctx->allocations[i].allocated) {
      TRACE_ALLOC_PORT_LOG(
        ANSI_TEXT_BOLD "malloc_checked" ANSI_TEXT_RESET ": "
          ANSI_COLOR_FG_RED "leak" ANSI_TEXT_RESET " %p %zu (allocated at "
          TRACE_ALLOC_LOC_FMT ")\n",
        ctx->allocations[i].info.ptr, ctx->allocations[i].info.size,
        ctx->allocations[i].location.fn, ctx->allocations[i].location.line
      );

      leaks++;
    }
  }

  return leaks;
}

void * trace_alloc_impl(size_t size, const char * fn, size_t line) {
  trace_alloc_ctx_t * ctx = trace_alloc_get_instance();

  void * ptr = TRACE_ALLOC_PORT_ALLOC_FN(size);

  if (ctx->enabled) {
    for (size_t i = 0; i < TRACE_ALLOC_BUF_SIZE; ++i) {
      if (!ctx->allocations[i].allocated) {
        ctx->allocations[i].info.ptr = ptr;
        ctx->allocations[i].info.size = size;
        ctx->allocations[i].location.fn = fn;
        ctx->allocations[i].location.line = line;
        ctx->allocations[i].allocated = true;

        TRACE_ALLOC_PORT_LOG(
          ANSI_TEXT_BOLD "malloc_checked" ANSI_TEXT_RESET ": %p %zu at "
            TRACE_ALLOC_LOC_FMT "\n",
          ptr, size, fn, line
        );

        break;
      }
    }
  }

  return ptr;
}

void trace_free_impl(void * ptr, const char * fn, size_t line) {
  trace_alloc_ctx_t * ctx = trace_alloc_get_instance();

  TRACE_ALLOC_PORT_FREE_FN(ptr);

  if (ctx->enabled) {
    for (size_t i = 0; i < TRACE_ALLOC_BUF_SIZE; ++i) {
      if (ctx->allocations[i].allocated && ctx->allocations[i].info.ptr == ptr) {
        ctx->allocations[i].allocated = false;

        TRACE_ALLOC_PORT_LOG(
          ANSI_TEXT_BOLD "free_checked" ANSI_TEXT_RESET ": %p %zu at "
            TRACE_ALLOC_LOC_FMT " (allocated at " TRACE_ALLOC_LOC_FMT ")\n",
          ptr, ctx->allocations[i].info.size,
          ctx->allocations[i].location.fn, ctx->allocations[i].location.line,
          fn, line
        );

        break;
      }
    }
  }
}

#if TRACE_ALLOC_USE_SINGLE_INSTANCE
trace_alloc_ctx_t * trace_alloc_get_instance(void) {
  return &trace_alloc_instance;
}
#endif


