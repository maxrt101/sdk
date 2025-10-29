/** ========================================================================= *
 *
 * @file builtin_repeat.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'repeat' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "shell/shell_util.h"
#include "bsp.h"
#include "log/log.h"
#include "wdt/wdt.h"
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
int8_t builtin_repeat(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc != 3) {
    log_error("Usage: repeat N \"COMMAND\"");
    return SHELL_FAIL;
  }

  int times = shell_parse_int(argv[1]);

  char command[SHELL_MAX_LINE_SIZE];
  strcpy(command, argv[2]);

  while (times--) {
    wdt_feed();
    int8_t result = shell_execute(sh, command);
    ASSERT_RETURN(result == 0, result);
  }

  return SHELL_OK;
}
