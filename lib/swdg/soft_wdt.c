/** ========================================================================= *
*
 * @file soft_wdt.c
 * @date 18-04-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "swdg/soft_wdt.h"
#include "error/assertion.h"
#include "log/log.h"
#include "os/reset/reset.h"

/* Defines ================================================================== */
#define LOG_TAG swdt

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/**
 * Software WatchDogTimer Context
 */
static struct {
  void * ctx;
  timeout_t timeout;
  uint8_t action;
} soft_wdt;

/* Private functions ======================================================== */
/* Shared functions ========================================================= */
void soft_wdt_init(milliseconds_t timeout, uint8_t action, void * ctx) {
  timeout_start(&soft_wdt.timeout, timeout);
  soft_wdt.action = action;
  soft_wdt.ctx    = ctx;
}

void soft_wdt_feed(void) {
  timeout_restart(&soft_wdt.timeout);
}

void soft_wdt_check(void) {
  if (timeout_is_expired(&soft_wdt.timeout)) {
    if (soft_wdt.action & SWDT_ACTION_LOG) {
      log_warn("Timeout expired");
    }

    if (soft_wdt.action & SWDT_ACTION_NOTIFY) {
      soft_wdt_on_timeout(soft_wdt.ctx);
    }

    if (soft_wdt.action & SWDT_ACTION_REBOOT_WDT) {
      os_reset(OS_RESET_WDG);
    }

    if (soft_wdt.action & SWDT_ACTION_REBOOT_SOFT) {
      os_reset(OS_RESET_SOFT);
    }

    if (soft_wdt.action & SWDT_ACTION_REBOOT_HARD) {
      os_reset(OS_RESET_HARD);
    }
  }
}

__WEAK void soft_wdt_on_timeout(void * ctx) {
  // Does nothing
}
