/** ========================================================================= *
 *
 * @file pwm.h
 * @date 05-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Software PWM library
 *        led_t led0;
 *        led_init(&led0, GPIO_TYPE_BIND(LED0), GPIO_POL_POSITIVE);
 *        pwm_t pwm;
 *        pwm_init(&pwm, &led0);
 *        pwm_set_duty(&pwm, 50);
 *        while (1) {
 *          pwm_tick(&pwm);
 *        }
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "hal/gpio/gpio.h"
#include "error/error.h"
#include "time/time.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * PWM Context
 */
typedef struct {
  gpio_ctx_t * gpio;      /** GPIO to be used in PWM */
  uint8_t duty;           /** Current duty */
  uint8_t next_duty;      /** Next duty */
  milliseconds_t elapsed; /** Elapsed ms from cycle start */
  milliseconds_t last;    /** Time value from last pwm_tick call */
} pwm_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initialize PWM instance
 *
 * @param[in] pwm  Instance to be initialized
 * @param[in] gpio GPIO to use for PWM
 */
error_t pwm_init(pwm_t * pwm, gpio_ctx_t * gpio);

/**
 * Set next duty for PWM
 *
 * @param[in] pwm  PWM instance
 * @param[in] duty Duty to be set
 */
error_t pwm_set_duty(pwm_t * pwm, uint8_t duty);

/**
 * Ticks PWM
 *
 * @param[in] pwm PWM instance
 */
error_t pwm_tick(pwm_t * pwm);

#ifdef __cplusplus
}
#endif