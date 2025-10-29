/** ========================================================================= *
 *
 * @file builtin_reset.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'reset' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "log/log.h"
#include "shell/shell.h"
#include "os/reset/reset.h"

/* Defines ================================================================== */
#define LOG_TAG SHELL

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
int8_t builtin_reset(shell_t * sh, uint8_t argc, const char ** argv) {
  os_reset_method_t method = OS_RESET_SOFT;

  if (argc > 1) {
    if (!strcmp(argv[1], "soft")) {
      method = OS_RESET_SOFT;
    } else if (!strcmp(argv[1], "hard")) {
      method = OS_RESET_HARD;
    } else if (!strcmp(argv[1], "wdt")) {
      method = OS_RESET_WDG;
    } else {
      log_error("Unknown reset method");
      return SHELL_FAIL;
    }
  }

  os_reset(method);
  return SHELL_OK;
}
