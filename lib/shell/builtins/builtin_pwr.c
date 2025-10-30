/** ========================================================================= *
*
 * @file builtin_pwr.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'pwr' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "bsp.h"
#include "log/log.h"
#include "os/power/power.h"
#include <string.h>

/* Defines ================================================================== */
#define LOG_TAG shell

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
static os_power_mode_t str_to_power_mode(const char * str) {
  ASSERT_RETURN(str, OS_POWER_MODE_COUNT);

  if (!strcmp(str, "AUTO")) {
    return OS_POWER_MODE_AUTO;
  } else if (!strcmp(str, "FAST")) {
    return OS_POWER_MODE_FAST_SLEEP;
  } else if (!strcmp(str, "DEEP")) {
    return OS_POWER_MODE_DEEP_SLEEP;
  } else {
    return OS_POWER_MODE_COUNT;
  }
}

/* Shared functions ========================================================= */
int8_t builtin_pwr(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc < 3) {
    log_error("Usage: pwr block|unblock|enter FAST|DEEP");
    return SHELL_FAIL;
  }

  if (!strcmp(argv[1], "block")) {
    os_power_mode_block(str_to_power_mode(argv[2]), true);
  } else if (!strcmp(argv[1], "unblock")) {
    os_power_mode_block(str_to_power_mode(argv[2]), false);
  } else if (!strcmp(argv[1], "enter")) {
    os_power_mode_change(str_to_power_mode(argv[2]));
  } else {
    log_error("Usage: pwr block|unblock|enter FAST|DEEP");
    return SHELL_FAIL;
  }

  return SHELL_OK;
}
