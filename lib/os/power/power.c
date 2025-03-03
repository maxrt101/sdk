
/** ========================================================================= *
 *
 * @file power.c
 * @date 27-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Power Mode Control
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "os/power/power.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
static struct {
  os_power_mode_t current_power_mode;
  uint16_t skip_table;
  uint16_t block_table;
} power_ctx = {OS_POWER_MODE_NORMAL, 0, 0};

/* Private functions ======================================================== */
/* Shared functions ========================================================= */
error_t os_power_mode_change(os_power_mode_t mode) {
  if (UTIL_BIT_GET(power_ctx.skip_table, mode)) {
    power_ctx.skip_table = UTIL_BIT_SET(power_ctx.skip_table, mode);
    return E_CANCELLED;
  }

  if (UTIL_BIT_GET(power_ctx.block_table, mode)) {
    return E_CANCELLED;
  }

  os_power_mode_t prev = power_ctx.current_power_mode;
  power_ctx.current_power_mode = mode;
  error_t err = os_power_mode_change_port(prev, mode);
  return err;
}

error_t os_power_mode_skip_next(os_power_mode_t mode) {
  power_ctx.skip_table = UTIL_BIT_SET(power_ctx.skip_table, mode);
  return E_OK;
}

error_t os_power_mode_block(os_power_mode_t mode, bool block) {
  power_ctx.skip_table = block
      ? UTIL_BIT_SET(power_ctx.skip_table, mode)
      : UTIL_BIT_CLEAR(power_ctx.skip_table, mode);
  return E_OK;
}

__WEAK error_t os_power_mode_change_port(os_power_mode_t prev_mode, os_power_mode_t new_mode) {
  log_warn("os_power_mode_change_port has no implementation");
  log_info("transition %s to %s", os_power_mode_to_str(prev_mode), os_power_mode_to_str(new_mode));
  return E_NOTIMPL;
}

const char * os_power_mode_to_str(os_power_mode_t mode) {
  switch (mode) {
    case OS_POWER_MODE_NORMAL:     return "NORMAL";
    case OS_POWER_MODE_FAST_SLEEP: return "FAST_SLEEP";
    case OS_POWER_MODE_DEEP_SLEEP: return "DEEP_SLEEP";
    default:
      return "<?>";
  }
}