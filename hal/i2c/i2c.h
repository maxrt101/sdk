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
#include "hal/gpio/gpio.h"
#include "lib/error/error.h"
#include "util/compiler.h"

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

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initialize I2C
 *
 * @param[out] i1c handle to be initialized
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

#ifdef __cplusplus
}
#endif