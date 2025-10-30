/** ========================================================================= *
 *
 * @file builtin_log.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'log' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG shell

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
int8_t builtin_log(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc < 3) {
    log_error("Usage: log LEVEL ...");
    return SHELL_FAIL;
  }

  log_level_t level = log_level_from_str(argv[1]);

  for (uint8_t i = 2; i < argc; ++i) {
    log_fmt(__FILE__, __LINE__, level, "%s", argv[i]);
  }

  return SHELL_OK;
}
