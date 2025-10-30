/** ========================================================================= *
 *
 * @file builtin_input.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'input' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "shell/shell_util.h"
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
#if USE_SHELL_ENV
int8_t builtin_input(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc < 2) {
    log_error("Usage: input VAR [PROMPT]");
    return SHELL_FAIL;
  }

  const char * var = argv[1];
  const char * prompt = argc == 3 ? argv[2] : "> ";

  log_printf("%s", prompt);

  tty_line_t line;
  SHELL_ERR_REPORT_RETURN(tty_line_reset(&line), "tty_line_reset failed");
  SHELL_ERR_REPORT_RETURN(tty_read_line(&sh->tty, &line), "tty_read_line failed");

  SHELL_ERR_REPORT_RETURN(shell_env_set(sh, var, line.buf), "shell_env_set failed");

  return SHELL_OK;
}
#endif
