/** ========================================================================= *
 *
 * @file sh_cmd.h
 * @date 27-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Shell helpers & utilities
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "shell/shell.h"
#include <string.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Converts boolean to SHELL_OK/SHELL_FAIL
 */
#define SHELL_BOOL_TO_RES(__val) ((__val) ? SHELL_OK : SHELL_FAIL)

/**
 * Checks expr for errors, prints comment if not E_OK, returns SHELL_FAIL
 */
#define SHELL_ERR_REPORT_RETURN(__expr, __comment)                           \
  do {                                                                    \
    error_t err = (__expr);                                               \
    if (err != E_OK) {                                                    \
      log_error(__comment ": %s", error2str(err));                        \
      return SHELL_FAIL;                                                  \
    }                                                                     \
  } while (0)

/**
 * Checks expr, prints comment if not true, returns SHELL_FAIL
 */
#define SHELL_ASSERT_REPORT_RETURN(__expr, __comment)                        \
  do {                                                                    \
    if (!(__expr)) {                                                      \
      log_error(__comment);                                               \
      return SHELL_FAIL;                                                  \
    }                                                                     \
  } while (0)


/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
__STATIC_INLINE int shell_parse_int(const char * str) {
  return strtol(str, NULL, 0);
}

#ifdef __cplusplus
}
#endif