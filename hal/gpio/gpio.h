/** ========================================================================= *
 *
 * @file gpio.h
 * @date 20-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Generic GPIO HAL API, has no default implementation
 *
 * Board must define
 *    GPIO_PORT_STRUCT_IMPL - fields for gpio_t separated by ;
 *    GPIO_PORT_BIND - takes implementation defined GPIO name, and outputs
 *                     something that can be passed to READ/SET/CLEAR macros
 *    GPIO_PORT_TYPE_BIND - takes GPIO name, and outputs gpio_t
 *    GPIO_PORT_READ - reads pin state
 *    GPIO_PORT_SET - sets pin
 *    GPIO_PORT_CLEAR - clears pin
 *    GPIO_PORT_EXPAND_TYPE - expands gpio_t into something that can be passed
 *                            to READ/SET/CLEAR macros
 *    GPIO_PORT_SET_PIN_MODE - Sets pin mode (INPUT/OUTPUT)
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "hal_gpio.h"
#include "error/assertion.h"
#include "util/compiler.h"
#include <stdbool.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Binds port GPIO identifier to gpio_t struct
 */
#define GPIO_TYPE_BIND(...) GPIO_PORT_TYPE_BIND(__VA_ARGS__)

/**
 * Binds port GPIO identifier to port interpretation, that can be used in
 * GPIO_READ, GPIO_SET, GPIO_CLEAR macros
 */
#define GPIO_BIND(...) GPIO_PORT_BIND(__VA_ARGS__)

/**
 * Convert result of GPIO_BIND to gpio_t
 */
#define GPIO_TO_TYPE(...) GPIO_PORT_TO_TYPE(__VA_ARGS__)

/**
 * Reads pin state, uses result of GPIO_BIND
 */
#define GPIO_READ(...) GPIO_PORT_READ(__VA_ARGS__)

/**
 * Sets pin, uses result of GPIO_BIND
 */
#define GPIO_SET(...) GPIO_PORT_SET(__VA_ARGS__)

/**
 * Clears pin, uses result of GPIO_BIND
 */
#define GPIO_CLEAR(...) GPIO_PORT_CLEAR(__VA_ARGS__)

/**
 * Toggles pin, uses result of GPIO_BIND
 */
#define GPIO_TOGGLE(...) GPIO_PORT_TOGGLE(__VA_ARGS__)

/**
 * Sets gpio pin mode, uses result of GPIO_BIND
 *
 * @ref gpio_pin_mode_t
 */
#define GPIO_SET_PIN_MODE(...) GPIO_PORT_SET_PIN_MODE(__VA_ARGS__)

/* Enums ==================================================================== */
/**
 * GPIO Pin mode
 */
typedef enum {
  GPIO_PIN_MODE_INPUT = 0,
  GPIO_PIN_MODE_OUTPUT = 0,
} gpio_pin_mode_t;

/**
 * Physical GPIO polarity
 * GPIO_POL_POSITIVE means 1 has to be set on a GPIO pin for it to have state of 1
 * GPIO_POL_NEGATIVE means that 0 has to be set for the same effect
 */
typedef enum {
  GPIO_POL_POSITIVE,
  GPIO_POL_NEGATIVE,
} gpio_polarity_t;


/* Types ==================================================================== */
/**
 * GPIO, implementation defined
 */
typedef struct
{
  GPIO_PORT_STRUCT_IMPL
} gpio_t;

/**
 *
 */
typedef struct {
  gpio_t gpio;
  gpio_polarity_t pol;
} gpio_ctx_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Reads pin state, uses result of GPIO_TYPE_BIND
 */
__STATIC_INLINE bool gpio_read(gpio_t gpio) {
  return GPIO_READ(GPIO_PORT_EXPAND_TYPE(gpio));
}

/**
 * Sets pin, uses result of GPIO_TYPE_BIND
 */
__STATIC_INLINE void gpio_set(gpio_t gpio) {
  GPIO_SET(GPIO_PORT_EXPAND_TYPE(gpio));
}

/**
 * Clears pin, uses result of GPIO_TYPE_BIND
 */
__STATIC_INLINE void gpio_clear(gpio_t gpio) {
  GPIO_CLEAR(GPIO_PORT_EXPAND_TYPE(gpio));
}

/**
 * Toggle pin, uses result of GPIO_TYPE_BIND
 */
__STATIC_INLINE void gpio_toggle(gpio_t gpio) {
  GPIO_TOGGLE(GPIO_PORT_EXPAND_TYPE(gpio));
}

/**
 * Sets gpio pin mode, uses result of GPIO_TYPE_BIND
 *
 * @ref gpio_pin_mode_t
 */
__STATIC_INLINE void gpio_set_pin_mode(gpio_t gpio, gpio_pin_mode_t mode) {
  GPIO_SET_PIN_MODE(GPIO_PORT_EXPAND_TYPE(gpio), mode);
}

/**
 * Initializes GPIO context
 *
 * @param[in] ctx   GPIO Context (gpio + polarity)
 * @param[in] gpio  GPIO
 * @param[in] pol   Polarity
 */
__STATIC_INLINE error_t gpio_ctx_init(gpio_ctx_t * ctx, gpio_t gpio, gpio_polarity_t pol) {
  ASSERT_RETURN(ctx, E_NULL);

  ctx->gpio = gpio;
  ctx->pol = pol;

  return E_OK;
}

/**
 * Sets GPIO pin with regards to polarity
 *
 * @param[in] ctx GPIO Context (gpio + polarity)
 */
__STATIC_INLINE error_t gpio_ctx_set(gpio_ctx_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  ctx->pol == GPIO_POL_POSITIVE ? gpio_set(ctx->gpio) : gpio_clear(ctx->gpio);

  return E_OK;
}

/**
 * Reads GPIO pin with regards to polarity
 *
 * @param[in] ctx GPIO Context (gpio + polarity)
 */
__STATIC_INLINE bool gpio_ctx_read(gpio_ctx_t * ctx) {
  ASSERT_RETURN(ctx, false);

  bool state = gpio_read(ctx->gpio);

  return ctx->pol == GPIO_POL_POSITIVE ? state : !state;
}

/**
 * Clears GPIO pin with regards to polarity
 *
 * @param[in] ctx GPIO Context (gpio + polarity)
 */
__STATIC_INLINE error_t gpio_ctx_clear(gpio_ctx_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  ctx->pol == GPIO_POL_POSITIVE ? gpio_clear(ctx->gpio) : gpio_set(ctx->gpio);

  return E_OK;
}

#ifdef __cplusplus
}
#endif
