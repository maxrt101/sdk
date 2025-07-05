/** ========================================================================= *
 *
 * @file i2c.c
 * @date 05-07-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "hal/i2c/i2c.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG HAL_I2C

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
error_t i2c_detect(i2c_t * i2c, i2c_detect_result_t result) {
  ASSERT_RETURN(i2c && result, E_NULL);

  for (uint8_t i = 0; i < 128; ++i) {
    uint8_t data = 0;

    // TODO: Send alone should work
    //       Even better - should add an ability to select detection method
    //       AUTO being the default. Look at i2cdetect.c mode
    i2c_send(i2c, i, &data, 1);
    error_t err = i2c_recv(i2c, i, &data, 1);

    if (err == E_OK) {
      result[i / 8] = UTIL_BIT_SET(result[i / 8], i % 8);
    } else {
      result[i / 8] = UTIL_BIT_CLEAR(result[i / 8], i % 8);
    }
  }

  return E_OK;
}

error_t i2c_detect_dump(const i2c_detect_result_t result) {
  ASSERT_RETURN(result, E_NULL);

  log_printf("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n");

  for (uint8_t i = 0; i < 128; i += 16) {
    log_printf("%02x: ", i);

    for (uint8_t j = 0; j < 16; ++j) {
      uint8_t index = i + j;

      if (UTIL_BIT_GET(result[(index) / 8], (index) % 8)) {
        log_printf("%02x ", index);
      } else {
        log_printf("-- ");
      }
    }

    log_printf("\r\n");
  }

  return E_OK;
}

