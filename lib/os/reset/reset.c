/** ========================================================================= *
 *
 * @file reset.c
 * @date 19-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "os/reset/reset.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG RST

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
__WEAK __NO_RETURN void os_reset(os_reset_method_t method) {
  os_reset_port(method);
}

__WEAK __NO_RETURN void os_reset_port(os_reset_method_t method) {
  log_warn("os_reset_port has no implementation");
  log_info("%s reset requested", os_reset_method_to_str(method));
  while (1) {
    /* Hangs in an endless loop, if no implementation is present */
  }
}

os_reset_reason_t os_get_reset_reason(void) {
  return os_get_reset_reason_port();
}

__WEAK os_reset_reason_t os_get_reset_reason_port(void) {
  log_warn("os_reset_port has no implementation");
  return OS_RESET_REASON_UNK;
}

const char * os_reset_method_to_str(os_reset_method_t method) {
  switch (method) {
    case OS_RESET_HARD: return "HARD";
    case OS_RESET_SOFT: return "SOFT";
    case OS_RESET_WDG:  return "WDG";
    default:
      return "?";
  }
}

const char * os_reset_reason_to_str(os_reset_reason_t reason) {
  switch (reason) {
    case OS_RESET_REASON_UNK:     return "UNK";
    case OS_RESET_REASON_HW_RST:  return "HW_RST";
    case OS_RESET_REASON_SW_RST:  return "SW_RST";
    case OS_RESET_REASON_WDG:     return "WDG";
    case OS_RESET_REASON_WWDG:    return "WWDG";
    case OS_RESET_REASON_POR:     return "POR";
    case OS_RESET_REASON_BOR:     return "BOR";
    default:
      return "?";
  }
}
