/** ========================================================================= *
 *
 * @file max3010x.h
 * @date 31-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief MAX30100/MAX30102 Pulse Oxymeter / Heart Rate Monitor Driver
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "lib/error/error.h"
#include "hal/i2c/i2c.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/**
 * MAX3010X Mode
 *
 * @note MULTI_LED is only supported on MAX30102
 */
typedef enum {
  MAX3010X_MODE_HEART_RATE = 2,
  MAX3010X_MODE_SPO2_HR    = 3,
  MAX30102_MODE_MULTI_LED  = 7,
} max3010x_mode_t;

/**
 * Samples averaging
 *
 * @note Only supported on MAX30102
 */
typedef enum {
  MAX30102_SAMPLE_AVG_1  = 0,
  MAX30102_SAMPLE_AVG_2  = 1,
  MAX30102_SAMPLE_AVG_4  = 2,
  MAX30102_SAMPLE_AVG_8  = 3,
  MAX30102_SAMPLE_AVG_16 = 4,
  MAX30102_SAMPLE_AVG_32 = 5,
} max30102_sample_avg_t;

/**
 * ADC Range in nano amps
 *
 * @note Only supported on MAX30102
 */
typedef enum {
  MAX30102_ADC_RANGE_2K_nA  = 0,
  MAX30102_ADC_RANGE_4K_nA  = 1,
  MAX30102_ADC_RANGE_8K_nA  = 2,
  MAX30102_ADC_RANGE_16K_nA = 3,
} max30102_adc_range_t;

/**
 * Pulse width (time between LED pulses) & ADC resolution
 *
 * @note The value represents 2 metrics at the same time, as they're tied together
 *
 * @note For MAX30100, use max30102_pulse_width_adc_resolution_t for MAX30102
 */
typedef enum {
  MAX30100_PULSE_WIDTH_200_ADC_13_BIT  = 0, // Pulse width 200us
  MAX30100_PULSE_WIDTH_400_ADC_14_BIT  = 1, // Pulse width 400us
  MAX30100_PULSE_WIDTH_800_ADC_15_BIT  = 2, // Pulse width 800us
  MAX30100_PULSE_WIDTH_1600_ADC_16_BIT = 3, // Pulse width 1600us
} max30100_pulse_width_adc_resolution_t;

/**
 * Pulse width (time between LED pulses) & ADC resolution
 *
 * @note The value represents 2 metrics at the same time, as they're tied together
 *
 * @note For MAX30102, use max30100_pulse_width_adc_resolution_t for MAX30100
 */
typedef enum {
  MAX30102_PULSE_WIDTH_68_ADC_15_BIT  = 0, // Pulse width 69us
  MAX30102_PULSE_WIDTH_118_ADC_16_BIT = 1, // Pulse width 118us
  MAX30102_PULSE_WIDTH_215_ADC_17_BIT = 2, // Pulse width 215us
  MAX30102_PULSE_WIDTH_411_ADC_18_BIT = 3, // Pulse width 411us
} max30102_pulse_width_adc_resolution_t;

/**
 * Sample rate in Hz
 *
 * @note For MAX30100, use max30102_sample_rate_t for MAX30102
 */
typedef enum {
  MAX30100_SAMPLE_RATE_50_HZ   = 0,
  MAX30100_SAMPLE_RATE_100_HZ  = 1,
  MAX30100_SAMPLE_RATE_167_HZ  = 2,
  MAX30100_SAMPLE_RATE_200_HZ  = 3,
  MAX30100_SAMPLE_RATE_400_HZ  = 4,
  MAX30100_SAMPLE_RATE_600_HZ  = 5,
  MAX30100_SAMPLE_RATE_800_HZ  = 6,
  MAX30100_SAMPLE_RATE_1000_HZ = 7,
} max30100_sample_rate_t;

/**
 * Sample rate in Hz
 *
 * @note For MAX30102, use max30100_sample_rate_t for MAX30100
 */
typedef enum {
  MAX30102_SAMPLE_RATE_50_HZ   = 0,
  MAX30102_SAMPLE_RATE_100_HZ  = 1,
  MAX30102_SAMPLE_RATE_200_HZ  = 2,
  MAX30102_SAMPLE_RATE_400_HZ  = 3,
  MAX30102_SAMPLE_RATE_800_HZ  = 4,
  MAX30102_SAMPLE_RATE_1000_HZ = 5,
  MAX30102_SAMPLE_RATE_1600_HZ = 6,
  MAX30102_SAMPLE_RATE_3200_HZ = 7,
} max30102_sample_rate_t;

/**
 * Value returned by max3010x_process
 */
typedef enum {
  /** Nothing to do */
  MAX3010X_STATUS_IDLE = 0,

  /** Can read samples from FIFO */
  MAX3010X_STATUS_SAMPLES_READY,

  /** Can read temperature */
  MAX3010X_STATUS_TEMP_READY,
} max3010x_status_t;

/* Types ==================================================================== */
/**
 * Raw Sample from FIFO
 */
typedef struct {
  uint32_t ir;
  uint32_t red;
} max3010x_sample_t;

/**
 * Temperature
 */
typedef struct {
  uint8_t value;
  uint8_t frac;
} max3010x_temp_t;

/**
 * MAX3010X Driver Config
 */
typedef struct {
  /** I2C Handle for transport */
  i2c_t * i2c;

  struct {
    max30102_adc_range_t max30102;
  } adc_range;

  struct {
    max30100_pulse_width_adc_resolution_t max30100;
    max30102_pulse_width_adc_resolution_t max30102;
  } pulse_width;

  struct {
    max30100_sample_rate_t max30100;
    max30102_sample_rate_t max30102;
  } sample_rate;

  struct {
    uint8_t ir;
    uint8_t red;
  } current;

  max3010x_mode_t mode;
} max3010x_cfg_t;

/**
 * MAX3010X Driver Context
 */
typedef struct {
  /** I2C Handle for transport */
  i2c_t * i2c;

  /** Cached LED current */
  struct {
    uint8_t ir;
    uint8_t red;
  } current;

  /** Revision ID */
  uint8_t rev_id;

  /** Part ID */
  uint8_t part_id;

  /** IRQ Flags 1 */
  uint8_t irq_flags_1;

  /** IRQ Flags 2 (used only on MAX30102) */
  uint8_t irq_flags_2;
} max3010x_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Probe & initialize MAX3010X device
 *
 * @param max3010x Driver context
 * @param cfg Driver config
 */
error_t max3010x_init(max3010x_t * max3010x, max3010x_cfg_t * cfg);

/**
 * Reset MAX3010X device
 *
 * @param max3010x Driver context
 */
error_t max3010x_reset(max3010x_t * max3010x);

/**
 * Put MAX3010X device into shutdown mode
 *
 * @param max3010x Driver context
 */
error_t max3010x_shutdown(max3010x_t * max3010x);

/**
 * Set MAX3010X operation mode
 *
 * @param max3010x Driver context
 * @param mode Device mode
 */
error_t max3010x_set_mode(max3010x_t * max3010x, max3010x_mode_t mode);

/**
 * Set MAX3010X sample rate
 *
 * @note For MAX30100 use values from max30100_sample_rate_t as rate
 * @note For MAX30102 use values from max30102_sample_rate_t as rate
 *
 * @param max3010x Driver context
 * @param rate Sample rate
 */
error_t max3010x_set_sample_rate(max3010x_t * max3010x, uint8_t rate);

/**
 * Set MAX3010X LED Pulse Width (ADC Resolution)
 *
 * @note For MAX30100 use max30100_pulse_width_adc_resolution_t as pw
 * @note For MAX30102 use max30102_pulse_width_adc_resolution_t as pw
 *
 * @param max3010x Driver context
 * @param pw Pulse Width / ADC Resolution
 */
error_t max3010x_set_pulse_width(max3010x_t * max3010x, uint8_t pw);

/**
 * Set MAX30102 ADC Range
 *
 * @note Only for MAX30102
 *
 * @param max3010x Driver context
 * @param rng ADC Range
 */
error_t max30102_set_adc_range(max3010x_t * max3010x, max30102_adc_range_t rng);

/**
 * Configure MAX30102 FIFO Parameters
 *
 * @note Only for MAX30102
 *
 * @param max3010x Driver context
 * @param avg ADC Sample Averaging
 * @param a_full_cnt Number of items in FIFO, that will generate A_FULL (almost full) irq
 * @param rollover If enabled, FIFO will behave like a ring buffer when filled
 */
error_t max30102_configure_fifo(max3010x_t * max3010x, max30102_sample_avg_t avg, uint8_t a_full_cnt, bool rollover);

/**
 * Set max current for IR LED
 *
 * @param max3010x Driver context
 * @param ma Current of IR LED in milli-amps
 */
error_t max3010x_set_ir_led_current(max3010x_t * max3010x, uint8_t ma);

/**
 * Set max current for RED LED
 *
 * @param max3010x Driver context
 * @param ma Current of RED LED in milli-amps
 */
error_t max3010x_set_red_led_current(max3010x_t * max3010x, uint8_t ma);

/**
 * Read temperature from sensor
 *
 * @param max3010x Driver context
 * @param temp Where to put temp
 */
error_t max3010x_read_temp(max3010x_t * max3010x, max3010x_temp_t * temp);

/**
 * Read samples from FIFO
 *
 * @param max3010x Driver context
 * @param samples Sample buffer
 * @param size On input - pointer to variable containing size of samples buffer;
 *             On output - value will be overwritten by number of written samples
 */
error_t max3010x_read_samples(max3010x_t * max3010x, max3010x_sample_t * samples, size_t * size);

/**
 * Poll IRQ flag registers
 *
 * Can be called in a loop, or in EXTI handler
 *
 * @param max3010x Device context
 */
error_t max3010x_poll_irq_flags(max3010x_t * max3010x);

/**
 * Process IRQ Flags
 *
 * @param max3010x Driver context
 * @returns MAX3010X_STATUS_SAMPLES_READY if max3010x_read_samples can be called to read samples from FIFO
 */
max3010x_status_t max3010x_process(max3010x_t * max3010x);

/**
 * Return minimal IR LED ADC value, based on supplied LED current
 *
 * Can be useful by using returned value as a threshold of raw ADC value
 *
 * @note Returning values were roughly estimated by hand
 *
 * @param max3010x Driver context
 * @return Minimal ADC value that can mean something
 */
uint16_t max3010x_get_min_ir_adc_voltage(max3010x_t * max3010x);

#ifdef __cplusplus
}
#endif