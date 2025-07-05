/** ========================================================================= *
 *
 * @file ds28.h
 * @date 23-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief DS28 Driver
 *
 * TODO: Separate generic DS28 logic from chain-mode
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "hal/onewire/onewire.h"
#include "error/error.h"

/* Defines ================================================================== */
/**
 * Can be passed to functions (convert_temp, read_temp, etc.)
 * to select all devices
 */
#define DS28_TARGET_ALL NULL

/**
 * Max devices (size of devices buffer)
 */
#ifndef DS28_MAX_DEVICES
#define DS28_MAX_DEVICES 4
#endif

/**
 * Temperature precision value
 * 1000 = 10^3 = 3 digits after point
 */
#ifndef DS28_PRECISION
#define DS28_PRECISION 1000
#endif

/**
 * Temperature scaler value
 * Temperature read returns 2 bytes, which have to be scaled correctly
 * (divide by DS28_SCALER)
 */
#ifndef DS28_SCALER
#define DS28_SCALER 16
#endif

/* Macros =================================================================== */
/* Enums ==================================================================== */
/**
 * DS28 Power Mode
 *   1W - device uses OneWire line for parasitic power supply
 *   VDD - device uses dedicated power supply line
 */
typedef enum {
  DS28_PWR_MODE_1W  = 0,
  DS28_PWR_MODE_VDD = 0xFF,
} ds28_pwr_mode_t;

/**
 * DS28 Temperature Resolution
 */
typedef enum {
  DS28_RESOLUTION_9_BIT  = 0x1F,
  DS28_RESOLUTION_10_BIT = 0x3F,
  DS28_RESOLUTION_11_BIT = 0x5F,
  DS28_RESOLUTION_12_BIT = 0x7F
} ds28_resolution_t;

/**
 * DS28 Driver Init Mode
 *   DEFAULT uses OneWire search algorithm
 *   CHAIN uses DS28 chain detection algorithm
 */
typedef enum {
  DS28_INIT_MODE_DEFAULT = 0,
  DS28_INIT_MODE_CHAIN
} ds28ae00_init_mode_t;

/* Types ==================================================================== */
/**
 * Represents DS28 device
 */
typedef struct {
  /** OneWire Device context */
  onewire_device_t ow_dev;

  /** Position of the device in the 1W chain */
  uint32_t chain_position;

  /** Power mode of DS28 (it can be powered from dedicated VDD or from 1W line) */
  ds28_pwr_mode_t pwr_mode;

  /** Configurable contents of scratchpad */
  struct {
    struct {
      union {
        uint16_t value;
        struct {
          uint8_t hi;
          uint8_t lo;
        };
      } temp_alarm;
      uint8_t config_register;
    };
  } scratchpad;
} ds28_device_t;

/**
 * Context of DS28 driver
 */
typedef struct {
  onewire_t * ow;

  struct {
    ds28_device_t items[DS28_MAX_DEVICES];
    size_t size;
  } devices;
} ds28_t;

/**
 * Represents DS28 temperature reading in celsius
 * A fixed point value (value.fraction)
 */
typedef struct {
  uint16_t value;
  uint16_t fraction;
} ds28_temp_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes DS28 driver
 * Detects all devices on the ow bus that have DS28 family code
 *
 * @param[in] ctx Driver context
 * @param[in] ow Initialized OneWire driver context
 */
error_t ds28_init(ds28_t * ctx, onewire_t * ow,
                      ds28ae00_init_mode_t init_mode);

/**
 * Deinitializes DS28 driver
 *
 * @param[in] ctx Driver context
 */
error_t ds28_deinit(ds28_t * ctx);

/**
 * Searches for DS28 devices on the bus
 *
 * @param[in] ctx Driver context
 * @param[out]    devices Pointer to array of devices
 * @param[in/out] size Pointer to devices array size,
 *                     outputs number of found devices
 */
error_t ds28_detect(ds28_t * ctx, ds28_device_t * devices,
                        size_t * size);

/**
 * Detects sequence of DS28 devices
 *
 * @param[in] ctx Driver context
 * @param[out]     devices Pointer to array of devices
 * @param[in/out]  size Pointer to devices array size,
 *                      outputs number of found devices
 */
error_t ds28_sequence_detect(ds28_t * ctx, ds28_device_t * devices,
                                 size_t * size);

/**
 * Issues convert temp command to target device
 *
 * @param[in] ctx Driver context
 * @param[in] target Target to send command to (can be DS28_TARGET_ALL
 *                   to send to all devices)
 */
error_t ds28_convert_temp(ds28_t * ctx, ds28_device_t * target);

/**
 * Gathers temp reading from target device
 *
 * @param[in] ctx Driver context
 * @param[in] target Target to send command to (can be DS28_TARGET_ALL
 *                   to send to all devices)
 * @prarm[out] temp Result of temperature reading
 */
error_t ds28_read_temp(ds28_t * ctx, ds28_device_t * target,
                           ds28_temp_t * temp);

/**
 * Reads power mode from target device
 *
 * @param[in] ctx Driver context
 * @param[in] target Target to send command to (can be DS28_TARGET_ALL
 *                   to send to all devices)
 * @prarm[out] pwr Power mode (see @ref ds28_pwr_mode_t)
 */
error_t ds28_read_power_mode(ds28_t * ctx, ds28_device_t * target,
                                 ds28_pwr_mode_t * pwr);

/**
 * Sets temperature alarm
 *
 * @param[in] ctx Driver context
 * @param[in] target Target to send command to
 * @prarm[in] alarm_temp Alarm temperature value
 */
error_t ds28_set_alarm(ds28_t * ctx, ds28_device_t * target,
                           uint16_t alarm_temp);

/**
 * Sets temperature alarm LO byte
 *
 * @param[in] ctx Driver context
 * @param[in] target Target to send command to
 * @prarm[in] alarm_temp_lo Alarm temperature LO byte value
 */
error_t ds28_set_alarm_lo(ds28_t * ctx, ds28_device_t * target,
                              uint8_t alarm_temp_lo);

/**
 * Sets temperature alarm HI byte
 *
 * @param[in] ctx Driver context
 * @param[in] target Target to send command to
 * @prarm[in] alarm_temp_hi Alarm temperature HI byte value
 */
error_t ds28_set_alarm_hi(ds28_t * ctx, ds28_device_t * target,
                              uint8_t alarm_temp_hi);

/**
 * Sets temperature resolution (@ref )
 *
 * @param[in] ctx Driver context
 * @param[in] target Target to send command to
 * @prarm[in] resolution Temperature resolution
 */
error_t ds28_set_resolution(ds28_t * ctx, ds28_device_t * target,
                                ds28_resolution_t resolution);

#ifdef __cplusplus
}
#endif