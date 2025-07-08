/** ========================================================================= *
 *
 * @file aht.h
 * @date 06-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief AHT10/15/20 Driver
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
#ifndef AHT_TEMP_PRECISION
#define AHT_TEMP_PRECISION 1000
#endif

/**
 * Humidity precision value
 * 1000 = 10^3 = 3 digits after point
 */
#ifndef AHT_HUMIDITY_PRECISION
#define AHT_HUMIDITY_PRECISION 1000
#endif

/**
 * Verbose logs for AHT
 */
#ifndef AHT_VERBOSE
#define AHT_VERBOSE 0
#endif

/* Macros =================================================================== */
/* Enums ==================================================================== */
/**
 * AHT Sensor Type (AHT10/AHT20)
 */
typedef enum {
  AHT_UNKNOWN,
  AHT_10,
  AHT_20,
} aht_type_t;

/* Types ==================================================================== */
/**
 * AHT Device context
 */
typedef struct {
  /** I2C Bus Context */
  i2c_t * i2c;

  /** AHT Device Address */
  uint16_t addr;

  /** AHT Type */
  aht_type_t type;
} aht_t;

/**
 * AHT Measurement result
 */
typedef struct {
  /**
   * Represents AHT temperature reading in celsius
   * A fixed point value (value.fraction)
   */
  struct {
    int16_t value;
    uint16_t fraction;
  } temp;

  /**
   * Represents AHT humidity reading in percent
   * A fixed point value (value.fraction)
   */
  struct {
    uint16_t value;
    uint16_t fraction;
  } humidity;
} aht_measurement_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializer AHT driver
 *
 * @param ctx Driver context
 * @param i2c Initialized I2C driver context
 * @param addr AHT device address
 */
error_t aht_init(aht_t * ctx, i2c_t * i2c, uint16_t addr);

/**
 * Deinitializes AHT driver
 *
 * @param ctx Driver context
 */
error_t aht_deinit(aht_t * ctx);

/**
 * Resets AHT device
 *
 * @param ctx Driver context
 */
error_t aht_reset(aht_t * ctx);

/**
 * Issues measure command to AHT device
 *
 * @param ctx Driver context
 */
error_t aht_measure(aht_t * ctx);

/**
 * Reads temperature and humidity values from AHT
 *
 * @note Needs aht_measure called before
 *
 * @param ctx Driver context
 * @param measurement Measurement (humidity + temp in fixed point format)
 */
error_t aht_read(aht_t * ctx, aht_measurement_t * measurement);

/**
 * Convert AHT sensor type enum value to string
 *
 * @param type AHT type
 */
const char * aht_type_to_str(aht_type_t type);

#ifdef __cplusplus
}
#endif