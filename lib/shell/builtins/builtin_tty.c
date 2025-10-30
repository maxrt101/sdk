/** ========================================================================= *
*
 * @file builtin_tty.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'tty' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "tty/tty.h"
#include "log/log.h"
#include "bsp.h"
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
int8_t builtin_tty(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc < 2) {
    log_error("Usage: tty echo|bell|reset");
    return SHELL_FAIL;
  }

  if (!strcmp(argv[1], "echo")) {
    if (tty_get_flag(&sh->tty, TTY_FLAG_ECHO_INPUT)) {
      tty_clear_flag(&sh->tty, TTY_FLAG_ECHO_INPUT);
    } else {
      tty_set_flag(&sh->tty, TTY_FLAG_ECHO_INPUT);
    }
  } else if (!strcmp(argv[1], "bell")) {
    if (tty_get_flag(&sh->tty, TTY_FLAG_USE_BELL)) {
      tty_clear_flag(&sh->tty, TTY_FLAG_USE_BELL);
    } else {
      tty_set_flag(&sh->tty, TTY_FLAG_USE_BELL);
    }
  } else if (!strcmp(argv[1], "reset")) {
    tty_reset(&sh->tty);
  } else {
    log_error("Unknown arg");
    return SHELL_FAIL;
  }

  return SHELL_OK;
}
