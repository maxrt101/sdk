/** ========================================================================= *
 *
 * @file abort.c
 * @date 20-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "os/abort/abort.h"
#include "os/reset/reset.h"
#include "log/log.h"
#include "log/color.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
__NO_RETURN void os_abort(const char * msg, ...) {
  va_list args;
  va_start(args, msg);
  log_fatal("%s        SYSTEM ABORT        %s", COLOR_RED_BG, COLOR_RESET);
  vlog_fmt(LOG_FATAL, msg, args);
  va_end(args);
  os_abort_dump_ctx();
  os_reset(OS_RESET_WDG);
}

__WEAK void os_abort_dump_ctx(void) {
  // Does nothing by default
}
