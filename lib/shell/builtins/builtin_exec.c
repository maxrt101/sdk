/** ========================================================================= *
 *
 * @file builtin_exec.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'exec' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "log/log.h"
#include <string.h>

/* Defines ================================================================== */
#define LOG_TAG shell

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
int8_t builtin_exec(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc != 2) {
    log_error("Usage: exec \"COMMAND\"");
    return SHELL_FAIL;
  }

  char command[SHELL_MAX_LINE_SIZE];
  strcpy(command, argv[1]);

  return shell_execute(sh, command);
}
