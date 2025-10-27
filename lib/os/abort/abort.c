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
  vlog_fmt(__FILE__, __LINE__, LOG_FATAL, NULL, msg, args);
  va_end(args);
  os_on_abort();
  os_reset(OS_RESET_WDG);
}

__WEAK void os_on_abort(void) {
  // Does nothing by default
}
