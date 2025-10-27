/** ========================================================================= *
 *
 * @file pwm.c
 * @date 05-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "pwm/pwm.h"
#include "error/assertion.h"
#include "util/util.h"
#include "time/time.h"

/* Defines ================================================================== */
#define LOG_TAG             pwm
#define PWM_FREQ_HZ         50
#define PWM_TICK_RATE       1000
#define PWM_MAX_DUTY_CYCLE  (PWM_TICK_RATE/PWM_FREQ_HZ)

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
static void pwm_start_next_cycle(pwm_t * pwm) {
  pwm->elapsed = 0;
  pwm->duty = pwm->next_duty;

  if (pwm->duty == 0) {
    gpio_ctx_clear(pwm->gpio);
  } else if (pwm->duty == PWM_MAX_DUTY_CYCLE) {
    gpio_ctx_set(pwm->gpio);
  }
}

/* Shared functions ========================================================= */
error_t pwm_init(pwm_t * pwm, gpio_ctx_t * gpio) {
  ASSERT_RETURN(pwm, E_NULL);

  pwm->gpio = gpio;
  pwm->next_duty = 0;
  pwm->duty = 0;
  pwm->elapsed = 0;
  pwm->last = 0;

  return E_OK;
}

error_t pwm_set_duty(pwm_t * pwm, uint8_t duty) {
  ASSERT_RETURN(pwm, E_NULL);

  pwm->next_duty = UTIL_MAP_RANGE(duty, 0, 100, 0, PWM_MAX_DUTY_CYCLE);

  return E_OK;
}

error_t pwm_tick(pwm_t * pwm) {
  ASSERT_RETURN(pwm, E_NULL);

  if (pwm->last == runtime_get()) {
    return E_AGAIN;
  }

  milliseconds_t last_time = pwm->last;
  pwm->last = runtime_get();

  if (pwm->elapsed == pwm->duty) {
    gpio_ctx_clear(pwm->gpio);
  } else if (!pwm->elapsed) {
    gpio_ctx_set(pwm->gpio);
  }

  pwm->elapsed += pwm->last - last_time;

  if (pwm->elapsed >= PWM_MAX_DUTY_CYCLE) {
    pwm_start_next_cycle(pwm);
  }

  return E_OK;
}
