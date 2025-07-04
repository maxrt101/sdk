/** ========================================================================= *
 *
 * @file i2c.h
 * @date 06-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Generic I2C HAL API, has no default implementation
 *
 * Board must implement init/deinit/send/recv,
 * using i2c_t* as void* pointer for device specific I2C context/handle
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdint.h>
#include <stddef.h>
#include "lib/error/error.h"
#include "error/assertion.h"
#include "util/compiler.h"
#include "util/bits.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Generic I2C handle
 */
typedef void i2c_handle_t;

/**
 * I2C Context
 */
typedef struct {
  i2c_handle_t * handle; /** Platform specific handle */
} i2c_t;

/**
 * I2C config
 */
typedef struct {
  uint8_t i2c_no;        /** I2C Peripheral Number */
} i2c_cfg_t;

/**
 * I2C Bus Detect Result
 *
 * This is a bitmap of device presence, each device is a bit
 * 1 - device is present at the address corresponding to offset
 * into the bitmap, 0 - device is absent
 * I2C can have 128 devices on the bus, so 16 bytes
 */
typedef uint8_t i2c_detect_result_t[16];

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initialize I2C
 *
 * @param[out] i2c handle to be initialized
 * @param[in] cfg I2C config
 */
error_t i2c_init(i2c_t * i2c, i2c_cfg_t * cfg);

/**
 * Deinitialize I2C
 *
 * @param[in] i2c handle to be deinitialized
 */
error_t i2c_deinit(i2c_t * i2c);

/**
 * Send buffer over I2C
 *
 * @param[in] i2c I2C handle
 * @param[in] addr I2C Device Address
 * @param[in] data Data buffer
 * @param[in] size Data buffer size
 */
error_t i2c_send(i2c_t * i2c, uint16_t addr, uint8_t * data, size_t size);

/**
 * Receive buffer over I2C
 *
 * @param[in] i2c I2C handle
 * @param[in] addr I2C Device Address
 * @param[in] data Data buffer
 * @param[in] size Data buffer size
 */
error_t i2c_recv(i2c_t * i2c, uint16_t addr, uint8_t * data, size_t size);

/**
 * Detect devices on I2C bus
 *
 * See @ref i2c_detect_result_t
 *
 * @param[in] i2c I2C handle
 * @param[in] result Bitmap of addresses (index is address, 1 - device is present)
 */
error_t i2c_detect(i2c_t * i2c, i2c_detect_result_t result);

/**
 * Dump i2c_detect result (linux i2cdetect style)
 *
 * See @ref i2c_detect_result_t
 *
 * @param[in] result Bitmap of addresses (index is address, 1 - device is present)
 */
error_t i2c_detect_dump(const i2c_detect_result_t result);

/**
 * Get first detected device, if no devices were detected - returns 0xFF
 *
 * See @ref i2c_detect_result_t
 *
 * @param result Bitmap of addresses (index is address, 1 - device is present)
 */
__STATIC_INLINE uint8_t i2c_detect_get_first(const i2c_detect_result_t result) {
  ASSERT_RETURN(result, E_NULL);

  for (uint8_t i = 0; i < 128; ++i) {
    if (UTIL_BIT_GET(result[i/8], i % 8)) {
      return i;
    }
  }

  return 0xFF;
}

#ifdef __cplusplus
}
#endif