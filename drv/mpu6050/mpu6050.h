/** ========================================================================= *
 *
 * @file mpu6050.h
 * @date 04-11-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief MPU6050 Gyroscope / Accelerometer Driver
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "drv/mpu6050/mpu6050_regs.h"
#include "lib/error/error.h"
#include "hal/i2c/i2c.h"
#include <stdint.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * MPU6050 Config
 */
typedef struct {
  /** I2C Handle */
  i2c_t * i2c;

  /** Gyroscope sensitivity/range */
  mpu6050_gyro_fs_sel_t   gyro;

  /** Accelerometer sensitivity/range */
  mpu6050_accel_afs_sel_t accel;
} mpu6050_cfg_t;

/**
 * MPU6050 Driver Context
 */
typedef struct {
  i2c_t * i2c;
} mpu6050_t;

/**
 * MPU6050 Data
 */
typedef struct {
  /** Gyroscope data */
  struct {
    int16_t x;
    int16_t y;
    int16_t z;
  } gyro;

  /** Accelerometer data */
  struct {
    int16_t x;
    int16_t y;
    int16_t z;
  } accel;

  /** Temperature data */
  struct {
    int32_t val;
  } temp;
} mpu6050_measurement_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes MPU6050
 *
 * @param mpu6050 Driver context
 * @param cfg Driver config
 */
error_t mpu6050_init(mpu6050_t * mpu6050, mpu6050_cfg_t * cfg);

/**
 * Resets MPU6050 (software reset via PWR_MGMT_1 register)
 *
 * @param mpu6050 Driver context
 */
error_t mpu6050_reset(mpu6050_t * mpu6050);

/**
 * Transfers MPU6050 to sleep mode (uses PWR_MGMT_1 register)
 *
 * @param mpu6050 Driver context
 */
error_t mpu6050_sleep(mpu6050_t * mpu6050);

/**
 * Reads current gyro/accel measurements
 *
 * @param mpu6050 Driver context
 * @param data Where to put the measurement
 */
error_t mpu6050_measure(mpu6050_t * mpu6050, mpu6050_measurement_t * data);

#ifdef __cplusplus
}
#endif