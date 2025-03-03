/** ========================================================================= *
 *
 * @file sleep.c
 * @date 20-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @note Needs sleep_us_port implemented in bsp
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "time/sleep.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
void sleep_us_port(uint16_t time_us);

void sleep_ms(uint32_t ms) {
  for (uint32_t i = 0; i < ms; ++i) {
    sleep_us(1000);
  }
}

void sleep_us(uint32_t us) {
  sleep_us_port(us);
}