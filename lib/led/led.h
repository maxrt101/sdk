/** ========================================================================= *
 *
 * @file led.h
 * @date 25-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief LED library, allows for pattern execution e.g.:
 *        // Define led context
 *        led_t led_red;
 *        // Initialize LED with its GPIO & polarity
 *        led_init(&led_red, GPIO_TYPE_BIND(LED_RED), GPIO_POL_POSITIVE);
 *        // Define a pattern
 *        LED_DEFINE_PATTERN(startup,  // Pattern name
 *            LED_ON, 500,             // Turn LED on for 500 ms
 *            LED_OFF, 100             // Turn LED off for 100 ms
 *            LED_FADE, 0, 100, 500    // Fade up from 0% to 100% in 500 ms
 *            LED_FADE_HOLD, 100, 500  // Hold 100% of brightness for 500 ms
 *            LED_FADE, 100, 0, 500    // Fade down from 100% to 0% in 500 ms
 *            LED_OFF, 100             // Keep LED off for 100 ms
 *        );
 *        // Set pattern
 *        led_schedule(&led_red, LED_PATTERN(startup));
 *        // Run pattern
 *        led_run(&led_red);
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "pwm/pwm.h"
#include "time/timeout.h"
#include "queue/queue.h"
#include "util/vargs.h"

/* Defines ================================================================== */
/**
 * Marks indefinite repat
 */
#define LED_REPEAT_INDEFINITELY 0xFFFF

/* Macros =================================================================== */
/**
 * Constructs variable name for pattern context
 *
 * @param name Pattern name
 */
#define LED_PATTERN_NAME(name) \
  UTIL_CAT(name, _led_pattern)

/**
 * Returns pointer to pattern
 *
 * @param name Pattern name
 */
#define LED_PATTERN(name) \
    &LED_PATTERN_NAME(name)

/**
 * Construct variable name for pattern commands array
 *
 * @param name Pattern name
 */
#define LED_PATTERN_COMMANDS(name) \
    UTIL_CAT(name, _led_pattern_commands)

/**
 * Declares pattern (for headers)
 *
 * @param name Pattern name
 */
#define LED_DECLARE_PATTERN(name) \
    extern led_pattern_t LED_PATTERN_NAME(name)

/**
 * Defines pattern (for sources)
 *
 * @param name Pattern name
 * @param ... Comma separated list of commands and their args
 */
#define LED_DEFINE_PATTERN(name, ...)                       \
    uint16_t LED_PATTERN_COMMANDS(name)[] = {__VA_ARGS__};  \
    led_pattern_t LED_PATTERN_NAME(name) = {                \
      LED_PATTERN_COMMANDS(name),                           \
      UTIL_ARR_SIZE(LED_PATTERN_COMMANDS(name))             \
    }

/* Enums ==================================================================== */
/**
 * Action to be performed
 */
typedef enum {
  LED_NONE = 0,   /** Does nothing */
  LED_ON,         /** Turns LED on, args: time_ms */
  LED_OFF,        /** Turns LED off, args: time_ms */
  LED_FADE,       /** Fades LED up/down, args: duty_from, duty_to, time_ms */
  LED_FADE_HOLD,  /** Fades LED down, args: duty_from, duty_to, time_ms */
  LED_REPEAT,     /** Repeats whole pattern from the beginning */
} led_action_t;

typedef enum {
  LED_STATE_IDLE = 0,             /** Idle state - does nothing */
  LED_STATE_READY,                /** Ready for next action */
  LED_STATE_EXECUTING,            /** Execution action (waits for the end) */
  LED_STATE_EXECUTING_FADE,       /** Executing fading action */
  LED_STATE_EXECUTING_FADE_HOLD,  /** Executing static fading action */
} led_state_t;

/* Types ==================================================================== */
/**
 * LED Pattern definition
 *
 * Basically an array with commands followed by their arguments
 */
typedef struct {
  uint16_t *  buffer;
  size_t      size;
} led_pattern_t;

/**
 * LED Context
 */
typedef struct led_s {
  gpio_ctx_t      gpio;         /** LED GPIO Context */
  pwm_t           pwm;          /** PWM For this LED */

  led_state_t     state;        /** LED state machine state */
  led_pattern_t * pattern;      /** Current LED pattern */
  queue_t *       queue;        /** Queue for patterns */

  bool            allow_repeat; /** Allows repeat command in pattens */

  /**
   * Context needed for pattern displaying
   */
  struct {
    uint8_t         repeat_count;    /** Counter of repeats */
    uint8_t         action_idx;      /** Index into pattern buffer */
    timeout_t       command_timeout; /** Timeout till end of command duration */
    milliseconds_t  last_runtime;    /** Runtime on last call */

    /**
     * Context needed for led fading
     */
    struct {
      enum {
        LED_FADE_DIR_UP,
        LED_FADE_DIR_DOWN
      } direction;       /** Fade direction */

      enum {
        LED_FADE_TYPE_TICKS,
        LED_FADE_TYPE_PERCENT
      } type;            /** Fade type */

      uint16_t from;     /** Duty percent to start from */
      uint16_t to;       /** Duty percent to end on */
      uint16_t time;     /** Time to reach target duty */

      uint16_t delta;    /** Delta |from - to| */
      uint16_t elapsed;  /** Elapsed time from start */
      uint16_t step;     /** Duty step */
    } fade;
  };
} led_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes LED context
 *
 * @param[in] led LED Context
 * @param[in] gpio Initialized GPIO struct that holds LED Pin info
 * @param[in] polarity LED polarity
 */
error_t led_init(led_t * led, gpio_t gpio, gpio_polarity_t polarity, queue_t * queue);

/**
 * Turns LED on with regards to polarity
 *
 * @param[in] led LED Context
 */
error_t led_on(led_t * led);

/**
 * Turns LED off with regards to polarity
 *
 * @param[in] led LED Context
 */
error_t led_off(led_t * led);

/**
 * Sets pattern to execute next
 *
 * @param[in] led LED Context
 * @param[in] pattern LED pattern pointer
 */
error_t led_schedule(led_t * led, led_pattern_t * pattern);

/**
 * Sets pattern to execute now (saving current pattern)
 *
 * @param[in] led LED Context
 * @param[in] pattern LED pattern pointer
 */
error_t led_preempt(led_t * led, led_pattern_t * pattern);

/**
 * Stops current led pattern
 *
 * @param led LED Context
 */
error_t led_stop(led_t * led);

/**
 * Flushes led pattern queue
 *
 * @param led LED Context
 */
error_t led_flush(led_t * led);

/**
 * Runs pattern to completion (blocking)
 *
 * @param[in] led LED Context
 */
error_t led_run(led_t * led);

/**
 * Runs 1 pattern action (if enough time passed)
 *
 * @param[in] led LED Context
 */
error_t led_run_async(led_t * led);

/**
 * Allow/disallow repeat command in patterns
 *
 * @param[in] led LED Context
 * @param[in] allow Allow flag
 */
error_t led_allow_repeat(led_t * led, bool allow);

#ifdef __cplusplus
}
#endif