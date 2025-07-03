
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
  uint16_t skip_table;
  uint16_t block_table;
} power_ctx = {0, 0};

/* Private functions ======================================================== */
static error_t os_power_mode_change_impl(os_power_mode_t mode) {
  if (UTIL_BIT_GET(power_ctx.skip_table, mode)) {
    power_ctx.skip_table = UTIL_BIT_SET(power_ctx.skip_table, mode);
    return E_CANCELLED;
  }

  if (UTIL_BIT_GET(power_ctx.block_table, mode)) {
    return E_CANCELLED;
  }

  return os_power_mode_change_port(mode);
}

/* Shared functions ========================================================= */
error_t os_power_mode_change(os_power_mode_t mode) {
  if (mode == OS_POWER_MODE_AUTO) {
    for (os_power_mode_t m = OS_POWER_MODE_COUNT - 1; m > OS_POWER_MODE_AUTO; --m) {
      if (os_power_mode_change_impl(m) == E_OK) {
        return E_OK;
      }
    }

    return E_CANCELLED;
  } else {
    return os_power_mode_change_impl(mode);
  }
}

error_t os_power_mode_skip_next(os_power_mode_t mode) {
  ASSERT_RETURN(mode < OS_POWER_MODE_COUNT, E_INVAL);

  power_ctx.skip_table = UTIL_BIT_SET(power_ctx.skip_table, mode);

  return E_OK;
}

error_t os_power_mode_block(os_power_mode_t mode, bool block) {
  ASSERT_RETURN(mode < OS_POWER_MODE_COUNT, E_INVAL);

  power_ctx.skip_table = block
      ? UTIL_BIT_SET(power_ctx.skip_table, mode)
      : UTIL_BIT_CLEAR(power_ctx.skip_table, mode);

  return E_OK;
}

__WEAK error_t os_power_mode_change_port(os_power_mode_t mode) {
  log_info("Transition to %s requested", os_power_mode_to_str(mode));
  log_warn("os_power_mode_change_port has no implementation");
  return E_NOTIMPL;
}

const char * os_power_mode_to_str(os_power_mode_t mode) {
  switch (mode) {
    case OS_POWER_MODE_AUTO:       return "AUTO";
    case OS_POWER_MODE_FAST_SLEEP: return "FAST_SLEEP";
    case OS_POWER_MODE_DEEP_SLEEP: return "DEEP_SLEEP";
    default:
      return "<?>";
  }
}