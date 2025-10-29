/** ========================================================================= *
*
 * @file builtin_sh.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'sh' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "log/log.h"
#include "bsp.h"
#include <string.h>

/* Defines ================================================================== */
#define LOG_TAG SHELL

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
int8_t builtin_sh(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc < 2) {
    log_error("Usage: sh +p|-p|+r|-r");
    return SHELL_FAIL;
  }

  if (!strcmp(argv[1], "+p")) {
    sh->flags |= SHELL_FLAG_SHOWPROMPT;
  } else if (!strcmp(argv[1], "-p")) {
    sh->flags &= ~SHELL_FLAG_SHOWPROMPT;
  } else if (!strcmp(argv[1], "+r")) {
    sh->flags |= SHELL_FLAG_ECHO_RES;
  } else if (!strcmp(argv[1], "-r")) {
    sh->flags &= ~SHELL_FLAG_ECHO_RES;
  } else {
    log_error("Unknown flag");
    return SHELL_FAIL;
  }

  return SHELL_OK;
}
