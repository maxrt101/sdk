/** ========================================================================= *
*
 * @file soft_wdg.c
 * @date 18-04-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "swdg/soft_wdg.h"
#include "error/assertion.h"
#include "log/log.h"
#include "os/reset/reset.h"

/* Defines ================================================================== */
#define LOG_TAG SWDG

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
void soft_wdg_init(soft_wdg_t * wdg, uint32_t max, uint8_t action, const char * label) {
  ASSERT_RETURN(wdg);

  wdg->label = label;
  wdg->max = max;
  wdg->action = action;
  wdg->counter = 0;
}

void soft_wdg_inc(soft_wdg_t * wdg) {
  ASSERT_RETURN(wdg);

  wdg->counter++;

  if (wdg->counter >= wdg->max) {
    if (wdg->action & SWDG_ACTION_LOG) {
      log_warn(wdg->label ? "Timeout expired for %s" : "Timeout expired", wdg->label);
    }

    if (wdg->action & SWDG_ACTION_NOTIFY) {
      soft_wdg_on_timeout(wdg);
    }

    if (wdg->action & SWDG_ACTION_REBOOT_WDT) {
      os_reset(OS_RESET_WDG);
    }

    if (wdg->action & SWDG_ACTION_REBOOT_SOFT) {
      os_reset(OS_RESET_SOFT);
    }

    if (wdg->action & SWDG_ACTION_REBOOT_HARD) {
      os_reset(OS_RESET_HARD);
    }
  }
}

void soft_wdg_reset(soft_wdg_t * wdg) {
  ASSERT_RETURN(wdg);

  wdg->counter = 0;
}

__WEAK void soft_wdg_on_timeout(soft_wdg_t * wdg) {
  // Does nothing
}
