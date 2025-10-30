/** ========================================================================= *
 *
 * @file builtin_env.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Builtin environment cli commands implementation
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
int8_t builtin_expr(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc != 4) {
    log_error("Usage: expr VAR +|-|*|/ VAL");
    return SHELL_FAIL;
  }

  const char * var = argv[1];
  const char * operation = argv[2];
  const char * value = argv[3];

  char * var_value;

  if (shell_env_find(sh, var, &var_value) != E_OK) {
    log_error("Variable '%s' doesn't exist", var);
    return SHELL_FAIL;
  }

  int var_int_value = shell_parse_int(var_value);
  int value_int = shell_parse_int(value);

  if (operation[0] == '+') {
    var_int_value += value_int;
  } else if (operation[0] == '-') {
    var_int_value -= value_int;
  } else if (operation[0] == '*') {
    var_int_value *= value_int;
  } else if (operation[0] == '/') {
    var_int_value /= value_int;
  } else {
    log_error("Invalid operation: use + - * /");
    return SHELL_FAIL;
  }

  itoa(var_int_value, var_value, 10);

  return SHELL_OK;
}

int8_t builtin_set(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc < 2) {
    log_error("Usage: set VARIABLE [VALUE]");
    return SHELL_FAIL;
  }

  shell_env_set(sh, argv[1], argc == 3 ? argv[2] : "");
  return SHELL_OK;
}

int8_t builtin_unset(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc != 2) {
    log_error("Usage: unset VARIABLE");
    return SHELL_FAIL;
  }

  shell_env_unset(sh, argv[1]);

  return SHELL_OK;
}

int8_t builtin_env(shell_t * sh, uint8_t argc, const char ** argv) {
  for (uint8_t i = 0; i < SHELL_VAR_BUFFER_SIZE; ++i) {
    if (sh->vars[i].used) {
      log_printf("%s=%s\r\n", sh->vars[i].name, sh->vars[i].value);
    }
  }
  return SHELL_OK;
}
#endif
