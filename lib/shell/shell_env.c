/** ========================================================================= *
 *
 * @file shell_env.c
 * @date 09-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Implementation of shell environment functionality
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <string.h>
#include "shell/shell.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG                   SHELL
#define SHELL_VAR_REFERENCE_MARK  '$'  /* Marks variable reference in a token */

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
#if USE_SHELL_ENV
error_t shell_env_find(shell_t * ctx, const char * name, char ** value) {
  ASSERT_RETURN(name && value, E_NULL);

  for (uint8_t i = 0; i < SHELL_VAR_BUFFER_SIZE; ++i) {
    if (ctx->vars[i].used && !strcmp(ctx->vars[i].name, name)) {
      *value = ctx->vars[i].value;
      return E_OK;
    }
  }

  *value = NULL;
  return E_NOTFOUND;
}

error_t shell_env_set(shell_t * ctx, const char * name, const char * value) {
  ASSERT_RETURN(name && value, E_NULL);

  // If variable with this name exists, find it and set the value
  for (uint8_t i = 0; i < SHELL_VAR_BUFFER_SIZE; ++i) {
    if (ctx->vars[i].used && !strcmp(ctx->vars[i].name, name)) {
      UTIL_STR_COPY(ctx->vars[i].value, value, SHELL_VAR_VALUE_SIZE);
      return E_OK;
    }
  }

  // If execution reached here - variable with the same name doesn't
  // exist, and new variable must be allocated
  for (uint8_t i = 0; i < SHELL_VAR_BUFFER_SIZE; ++i) {
    if (!ctx->vars[i].used) {
      UTIL_STR_COPY(ctx->vars[i].name, name, SHELL_VAR_NAME_SIZE);
      UTIL_STR_COPY(ctx->vars[i].value, value, SHELL_VAR_VALUE_SIZE);
      ctx->vars[i].used = true;
      return E_OK;
    }
  }

  return E_OVERFLOW;
}

error_t shell_env_unset(shell_t * ctx, const char * name) {
  ASSERT_RETURN(name, E_NULL);

  // Find variable and set used flag to 0 which marks it as unused
  for (uint8_t i = 0; i < SHELL_VAR_BUFFER_SIZE; ++i) {
    if (ctx->vars[i].used && !strcmp(ctx->vars[i].name, name)) {
      ctx->vars[i].used = false;
      return E_OK;
    }
  }

  return E_NOTFOUND;
}

const char * shell_arg_parse(shell_t * ctx, const char * arg) {
  if (!arg) {
    return "";
  }

  // If first char is a variable reference mark
  if (arg[0] == SHELL_VAR_REFERENCE_MARK) {
    char * value;
    // Find and return the value for this variable
    if (shell_env_find(ctx, &arg[1], &value) != E_OK) {
      // If undefined variable was referenced, return arg
      log_warn("Can't find variable '%s'", arg);
      return arg;
    }
    // Return variable value
    return value;
  }

  return arg;
}
#endif

