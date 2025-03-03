/** ========================================================================= *
 *
 * @file timeout.c
 * @date 20-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "time/timeout.h"
#include "error/assertion.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
void timeout_start(timeout_t * timeout, milliseconds_t ms) {
  ASSERT_RETURN(timeout);

  timeout->duration = ms;
  timeout->start = runtime_get();
}

void timeout_restart(timeout_t * timeout) {
  ASSERT_RETURN(timeout);

  timeout->start = runtime_get();
}

bool timeout_is_expired(const timeout_t * timeout) {
  ASSERT_RETURN(timeout, true);

  return runtime_get() >= (timeout->start + timeout->duration);
}

void timeout_expire(timeout_t * timeout) {
  ASSERT_RETURN(timeout);

  timeout->duration = 0;
}
