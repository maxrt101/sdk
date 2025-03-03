/** ========================================================================= *
 *
 * @file btn.c
 * @date 12-11-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Button library
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "btn/btn.h"
#include "error/assertion.h"

/* Defines ================================================================== */
#define LOG_TAG BTN

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
error_t btn_init(btn_t * btn, btn_cfg_t * cfg) {
  ASSERT_RETURN(btn && cfg, E_NULL);

  memset(btn, 0, sizeof(*btn));

  btn->gpio = cfg->gpio;
  btn->press_time = cfg->press_time;

  return E_OK;
}

error_t btn_poll(btn_t * btn) {
  ASSERT_RETURN(btn, E_NULL);

  bool state = gpio_ctx_read(&btn->gpio);

  if (btn->flags.last_phy_state && !state) {
    btn->flags.released = true;
  } else if (!btn->flags.last_phy_state && state) {
    timeout_start(&btn->press_timeout, btn->press_time);
  }

  if (state) {
    if (timeout_is_expired(&btn->press_timeout)) {
      btn->flags.pressed = true;
      timeout_restart(&btn->press_timeout);
    }
  }

  btn->flags.last_phy_state = state;

  return E_OK;
}

bool btn_was_pressed(btn_t * btn) {
  ASSERT_RETURN(btn, E_NULL);

  bool pressed = btn->flags.pressed;

  btn->flags.pressed = false;

  return pressed;
}

bool btn_was_released(btn_t * btn) {
  ASSERT_RETURN(btn, E_NULL);

  bool released = btn->flags.released;

  btn->flags.released = false;

  return released;
}
