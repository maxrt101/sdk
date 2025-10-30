/** ========================================================================= *
 *
 * @file builtin_history.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'history' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "error/assertion.h"
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
int8_t builtin_history(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc == 2 && !strcmp("clear", argv[1])) {
    shell_history_clear(sh);
    return SHELL_OK;
  }

  ASSERT_RETURN(sh->history.head != -1, SHELL_OK);

  // FIXME: There is currently no API to iterate over history is reverse order
  sh->history.index = (sh->history.head + 1) % SHELL_HISTORY_BUFFER_SIZE;

  while (1) {
    tty_line_t line;
    shell_history_get(sh, &line);
    log_printf("%s\r\n", tty_line_get_str(&line));

    if (sh->history.index == sh->history.head) {
      break;
    }

    sh->history.index = (sh->history.index + 1) % SHELL_HISTORY_BUFFER_SIZE;
  }

  shell_history_reset(sh);

  return SHELL_OK;
}
