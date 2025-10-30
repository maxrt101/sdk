/** ========================================================================= *
 *
 * @file builtin_check.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'check' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "shell/shell_util.h"
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
#if USE_SHELL_ENV
int8_t builtin_check(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc < 2) {
    log_error("Usage: check VAL1 [!=|==|>|<|<=|>=] [VAL2] ");
    return SHELL_FAIL;
  }

  const char * val1 = argv[1];

  if (argc == 4) {
    const char * operation = argv[2];
    const char * val2 = argv[3];

    if (!strcmp(operation, "==")) return SHELL_BOOL_TO_RES(!strcmp(val1, val2));
    if (!strcmp(operation, "!=")) return SHELL_BOOL_TO_RES(strcmp(val1, val2));
    if (!strcmp(operation, ">"))  return SHELL_BOOL_TO_RES(shell_parse_int(val1) > shell_parse_int(val2));
    if (!strcmp(operation, "<"))  return SHELL_BOOL_TO_RES(shell_parse_int(val1) > shell_parse_int(val2));
    if (!strcmp(operation, ">=")) return SHELL_BOOL_TO_RES(shell_parse_int(val1) >= shell_parse_int(val2));
    if (!strcmp(operation, "<=")) return SHELL_BOOL_TO_RES(shell_parse_int(val1) <= shell_parse_int(val2));

    return SHELL_FAIL;
  }

  char * value;
  return SHELL_BOOL_TO_RES(shell_env_find(sh, val1, &value) == E_OK);
}
#endif
