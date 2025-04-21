/** ========================================================================= *
 *
 * @file ds28.h
 * @date 06-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief AHT10 Driver
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "hal/i2c/i2c.h"
#include "error/error.h"

/* Defines ================================================================== */
/**
 * Temperature precision value
 * 1000 = 10^3 = 3 digits after point
 */
#ifndef AHT10_TEMP_PRECISION
#define AHT10_TEMP_PRECISION 1000
#endif

/**
 * Humidity precision value
 * 1000 = 10^3 = 3 digits after point
 */
#ifndef AHT10_HUMIDITY_PRECISION
#define AHT10_HUMIDITY_PRECISION 1000
#endif

/**
 * Verbose logs for AHT10
 */
#ifndef AHT10_VERBOSE
#define AHT10_VERBOSE 0
#endif

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * AHT10 Device context
 */
typedef struct {
  /** I2C Bus Context */
  i2c_t * i2c;

  /** AHT10 Device Address */
  uint16_t addr;
} aht10_t;

/**
 * AHT10 Measurement result
 */
typedef struct {
  /**
   * Represents AHT10 temperature reading in celsius
   * A fixed point value (value.fraction)
   */
  struct {
    int16_t value;
    uint16_t fraction;
  } temp;

  /**
   * Represents AHT10 humidity reading in percent
   * A fixed point value (value.fraction)
   */
  struct {
    uint16_t value;
    uint16_t fraction;
  } humidity;
} aht10_measurement_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializer AHT10 driver
 *
 * @param ctx Driver context
 * @param i2c Initialized I2C driver context
 * @param addr AHT10 device address
 */
error_t aht10_init(aht10_t * ctx, i2c_t * i2c, uint16_t addr);

/**
 * Deinitializes AHT10 driver
 *
 * @param ctx Driver context
 */
error_t aht10_deinit(aht10_t * ctx);

/**
 * Resets AHT10 device
 *
 * @param ctx Driver context
 */
error_t aht10_reset(aht10_t * ctx);

/**
 * Issues measure command to AHT10 device
 *
 * @param ctx Driver context
 */
error_t aht10_measure(aht10_t * ctx);

/**
 * Reads temperature and humidity values from AHT10
 *
 * @note Needs aht10_measure called before
 *
 * @param ctx Driver context
 * @param measurement Measurement (humidity + temp in fixed point format)
 */
error_t aht10_read(aht10_t * ctx, aht10_measurement_t * measurement);

#ifdef __cplusplus
}
#endif