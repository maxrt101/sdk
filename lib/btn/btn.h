/** ========================================================================= *
 *
 * @file btn.h
 * @date 12-11-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Button library
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
#include "time/timeout.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/** Button Context */
typedef struct {
  gpio_ctx_t     gpio;
  milliseconds_t press_time;
  timeout_t      press_timeout;
  struct {
    bool last_phy_state  : 1;
    bool pressed         : 1;
    bool released        : 1;
    bool pressed_timeout : 1;
  } flags;
} btn_t;

/** Button Config */
typedef struct {
  gpio_ctx_t     gpio;
  milliseconds_t press_time;
} btn_cfg_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 *
 */
error_t btn_init(btn_t * btn, btn_cfg_t * cfg);

/**
 *
 */
error_t btn_poll(btn_t * btn);

/**
 *
 */
bool btn_was_pressed_for_timeout(btn_t * btn);

/**
 *
 */
bool btn_was_pressed(btn_t * btn);

/**
 *
 */
bool btn_was_released(btn_t * btn);

#ifdef __cplusplus
}
#endif