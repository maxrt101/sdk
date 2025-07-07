/** ========================================================================= *
 *
 * @file led.c
 * @date 25-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "led/led.h"
#include "error/assertion.h"


/* Defines ================================================================== */
#define LOG_TAG             LED
#define LED_ACTION_END_MARK ((uint16_t) -1)

/* Macros =================================================================== */
/**
 * Called when LED_ASSERT failed
 *
 * @param r Return value
 * @param ... Optional state to set (LED_STATE_IDLE is default)
 */
#define SET_STATE_RETURN(r, ...)                                          \
    led->pattern = NULL;                                                  \
    led->state = UTIL_IF_EMPTY(__VA_ARGS__, LED_STATE_IDLE, __VA_ARGS__); \
    return r;

/**
 * LED Assert, resets state & LED pattern of failure
 *
 * @param e Expression to test
 * @param r Return value on fail
 * @param ... Optional state to set
 */
#define LED_ASSERT(e, r, ...) \
    ASSERT_OR_ELSE(e, SET_STATE_RETURN(r, __VA_ARGS__));

/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/**
 * Retrieves next command/argument (16 bits)
 *
 * @note Returns LED_ACTION_END_MARK if reached end of pattern
 */
__STATIC_INLINE uint16_t led_get_next(led_t * led) {
  ASSERT_RETURN(led->action_idx < led->pattern->size, LED_ACTION_END_MARK);
  return led->pattern->buffer[led->action_idx++];
}

/**
 * If action is done (action timeout expired) sets next state
 */
__STATIC_INLINE void led_check_action_done(led_t * led) {
  if (timeout_is_expired(&led->command_timeout)) {
    led->state = LED_STATE_READY;
  }
}

/**
 * Parses next command
 */
__STATIC_INLINE error_t led_parse_command(led_t * led) {
  switch (led_get_next(led)) {
    case LED_ON: {
      uint16_t ms = led_get_next(led);
      LED_ASSERT(ms != LED_ACTION_END_MARK, E_INVAL);
      timeout_start(&led->command_timeout, ms);
      led->state = LED_STATE_EXECUTING;
      return led_on(led);
    }

    case LED_OFF: {
      uint16_t ms = led_get_next(led);
      LED_ASSERT(ms != LED_ACTION_END_MARK, E_INVAL);
      timeout_start(&led->command_timeout, ms);
      led->state = LED_STATE_EXECUTING;
      return led_off(led);
    }

    case LED_RGB: {
      uint16_t r  = led_get_next(led);
      uint16_t g  = led_get_next(led);
      uint16_t b  = led_get_next(led);
      uint16_t ms = led_get_next(led);

      LED_ASSERT(r  != LED_ACTION_END_MARK, E_INVAL);
      LED_ASSERT(g  != LED_ACTION_END_MARK, E_INVAL);
      LED_ASSERT(b  != LED_ACTION_END_MARK, E_INVAL);
      LED_ASSERT(ms != LED_ACTION_END_MARK, E_INVAL);

      timeout_start(&led->command_timeout, ms);
      led->state = LED_STATE_EXECUTING;
      return led_rgb_ctl(led, r, g, b);
    }

    case LED_FADE: {
      led->fade.from = led_get_next(led);
      led->fade.to   = led_get_next(led);
      led->fade.time = led_get_next(led);

      LED_ASSERT(led->fade.from != LED_ACTION_END_MARK, E_EMPTY);
      LED_ASSERT(led->fade.to   != LED_ACTION_END_MARK, E_EMPTY);
      LED_ASSERT(led->fade.time != LED_ACTION_END_MARK, E_EMPTY);

      if (led->fade.to > led->fade.from) {
        led->fade.direction = LED_FADE_DIR_UP;
        led->fade.delta = led->fade.to - led->fade.from;
      } else {
        led->fade.direction = LED_FADE_DIR_DOWN;
        led->fade.delta = led->fade.from - led->fade.to;
      }

      if (led->fade.time > led->fade.delta) {
        /* Duration is bigger than target duty - duty has to be changed by 1%
         * every N ms */
        led->fade.step = led->fade.time / led->fade.delta;
        led->fade.type = LED_FADE_TYPE_TICKS;
      } else {
        /* Target duty is bigger than the duration - duty has to be changed
         * by N % every ms */
        led->fade.step = led->fade.delta / led->fade.time;
        led->fade.type = LED_FADE_TYPE_PERCENT;
      }

      led->state = LED_STATE_EXECUTING_FADE;
      led->fade.elapsed = 0;
      break;
    }

    case LED_FADE_HOLD: {
      uint16_t duty = led_get_next(led);
      uint16_t ms = led_get_next(led);

      ASSERT_RETURN(duty != LED_ACTION_END_MARK, E_INVAL);
      ASSERT_RETURN(ms != LED_ACTION_END_MARK, E_INVAL);

      pwm_set_duty(&led->pwm, duty);
      timeout_start(&led->command_timeout, ms);

      led->state = LED_STATE_EXECUTING_FADE_HOLD;

      break;
    }

    case LED_REPEAT: {
      uint16_t repeats = led_get_next(led);

      if (led->repeat_count < repeats - 1 || (led->allow_repeat && repeats == LED_REPEAT_INDEFINITELY)) {
        led->state = LED_STATE_READY;
        led->action_idx = 0;
        led->repeat_count++;
      } else {
        led->pattern = NULL;
        led->state = LED_STATE_IDLE;
      }
      break;
    }

    case LED_NONE: {
      return E_AGAIN;
    }

    default: {
      return E_INVAL;
    }
  }

  return E_OK;
}

static void led_init_common(led_t * led, queue_t * queue) {
  memset(led, 0, sizeof(*led));

  led->queue        = queue;
  led->allow_repeat = true;
  led->repeat_count = 0;
  led->action_idx   = 0;
}

/* Shared functions ========================================================= */
error_t led_init(led_t * led, gpio_t gpio, gpio_polarity_t polarity, queue_t * queue) {
  ASSERT_RETURN(led && queue, E_NULL);

  led_init_common(led, queue);

  gpio_ctx_init(&led->gpio, gpio, polarity);
  pwm_init(&led->pwm, &led->gpio);

  return E_OK;
}

error_t led_init_rgb(led_t * led, led_rgb_init_t * gpios, queue_t * queue) {
  ASSERT_RETURN(led && gpios && queue, E_NULL);

  led_init_common(led, queue);

  gpio_ctx_init(&led->rgb.r, gpios->r.gpio, gpios->r.polarity);
  gpio_ctx_init(&led->rgb.g, gpios->g.gpio, gpios->g.polarity);
  gpio_ctx_init(&led->rgb.b, gpios->b.gpio, gpios->b.polarity);

  return E_OK;
}

error_t led_on(led_t * led) {
  ASSERT_RETURN(led, E_NULL);

  return led->type == LED_TYPE_RGB ? led_rgb_ctl(led, 1, 1, 1) : gpio_ctx_set(&led->gpio);
}

error_t led_off(led_t * led) {
  ASSERT_RETURN(led, E_NULL);

  return led->type == LED_TYPE_RGB ? led_rgb_ctl(led, 0, 0, 0) : gpio_ctx_clear(&led->gpio);
}

error_t led_rgb_ctl(led_t * led, uint8_t r, uint8_t g, uint8_t b) {
  ASSERT_RETURN(led, E_NULL);

  // TODO: Create 3 PWM instances for each pin, and set brightness with them for each pin
  r ? gpio_ctx_set(&led->rgb.r) : gpio_ctx_clear(&led->rgb.r);
  g ? gpio_ctx_set(&led->rgb.g) : gpio_ctx_clear(&led->rgb.g);
  b ? gpio_ctx_set(&led->rgb.b) : gpio_ctx_clear(&led->rgb.b);

  return E_OK;
}

error_t led_schedule(led_t * led, led_pattern_t * pattern) {
  ASSERT_RETURN(led && pattern, E_NULL);

  ERROR_CHECK_RETURN(queue_push(led->queue, pattern));

  return E_OK;
}

error_t led_preempt(led_t * led, led_pattern_t * pattern) {
  ASSERT_RETURN(led && pattern, E_NULL);

  queue_push_front(led->queue, led->pattern);
  led->pattern = pattern;
  led->state = LED_STATE_READY;
  led->action_idx = 0;
  led->repeat_count = 0;
  led_off(led);

  return E_OK;
}

error_t led_stop(led_t * led) {
  ASSERT_RETURN(led, E_NULL);

  led->pattern = NULL;
  led->action_idx = 0;
  led_off(led);

  return E_OK;
}

error_t led_flush(led_t * led) {
  ASSERT_RETURN(led, E_NULL);

  return queue_clear(led->queue);
}

error_t led_run(led_t * led) {
  ASSERT_RETURN(led, E_NULL);

  do {
    led_run_async(led);
  } while (led->pattern || queue_size(led->queue));

  return E_OK;
}

error_t led_run_async(led_t * led) {
  ASSERT_RETURN(led, E_NULL);

  milliseconds_t runtime = runtime_get();
  milliseconds_t elapsed = runtime - led->last_runtime;

  if (elapsed < 1) {
    return E_AGAIN;
  }

  led->last_runtime = runtime;

  switch (led->state) {
    case LED_STATE_READY: {
      LED_ASSERT(led->pattern, E_NULL);

      if (led->action_idx >= led->pattern->size) {
        led->state = LED_STATE_IDLE;
        led->pattern = NULL;
        break;
      }

      error_t err = led_parse_command(led);
      if (err != E_OK) {
        led->state = LED_STATE_IDLE;
        led->pattern = NULL;
        return err;
      }

      break;
    }

    case LED_STATE_EXECUTING: {
      led_check_action_done(led);
      break;
    }

    case LED_STATE_EXECUTING_FADE: {
      if (led->fade.elapsed <= led->fade.time) {
        int duty = led->fade.from;

        if (led->fade.direction == LED_FADE_DIR_UP) {
          duty += led->fade.type == LED_FADE_TYPE_TICKS
                ? led->fade.elapsed / led->fade.step
                : led->fade.elapsed * led->fade.step;
        } else {
          duty += led->fade.type == LED_FADE_TYPE_TICKS
                ? led->fade.to - UTIL_CAP(led->fade.elapsed, 0, led->fade.time) / led->fade.step
                : led->fade.to - UTIL_CAP(led->fade.elapsed, 0, led->fade.time) * led->fade.step;
        }

        pwm_set_duty(&led->pwm, duty);
        pwm_tick(&led->pwm);

        led->fade.elapsed += elapsed;
      } else {
        led->state = LED_STATE_READY;
      }

      break;
    }

    case LED_STATE_EXECUTING_FADE_HOLD: {
      led_check_action_done(led);
      pwm_tick(&led->pwm);
      break;
    }

    case LED_STATE_IDLE:
      if (led->pattern) {
        led->state = LED_STATE_READY;
      } else if (queue_size(led->queue)) {
        ERROR_CHECK_RETURN(queue_pop(led->queue, (queue_element_t *) &led->pattern));
        led->state = LED_STATE_READY;
      }
      led->repeat_count = 0;
      led->action_idx = 0;
      return E_AGAIN;

    default:
      led->state = LED_STATE_IDLE;
      return E_INVAL;
  }

  return E_OK;
}

error_t led_allow_repeat(led_t * led, bool allow) {
  ASSERT_RETURN(led, E_NULL);

  led->allow_repeat = allow;

  return E_OK;
}

