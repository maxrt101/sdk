/** ========================================================================= *
 *
 * @file mpu6050.c
 * @date 03-11-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "drv/mpu6050/mpu6050_regs.h"
#include "drv/mpu6050/mpu6050.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG mpu6050

/* Macros =================================================================== */
#define MPU6050_VERBOSE 0

/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
static error_t mpu6050_reg_write(mpu6050_t * mpu6050, uint8_t reg, uint8_t val) {
#if MPU6050_VERBOSE
  log_debug("write %02x %02x", reg, val);
#endif

  return i2c_send(mpu6050->i2c, MPU6050_I2C_ADDR, (uint8_t[]){reg, val}, 2);
}

static error_t mpu6050_reg_read(mpu6050_t * mpu6050, uint8_t reg, uint8_t * val) {
  ERROR_CHECK_RETURN(i2c_send(mpu6050->i2c, MPU6050_I2C_ADDR, &reg, 1));
  ERROR_CHECK_RETURN(i2c_recv(mpu6050->i2c, MPU6050_I2C_ADDR, val, 1));

#if MPU6050_VERBOSE
  log_debug("read %02x %02x", reg, *val);
#endif

  return E_OK;
}

static error_t mpu6050_read_burst(mpu6050_t * mpu6050, uint8_t reg, uint8_t * data, uint8_t size) {
  ERROR_CHECK_RETURN(i2c_send(mpu6050->i2c, MPU6050_I2C_ADDR, &(uint8_t){reg}, 1));
  ERROR_CHECK_RETURN(i2c_recv(mpu6050->i2c, MPU6050_I2C_ADDR, data, size));
  return E_OK;
}

static uint8_t mpu6050_read_who_am_i(mpu6050_t * mpu6050) {
  uint8_t reg = 0;

  ASSERT_RETURN(mpu6050_reg_read(mpu6050, MPU6050_REG_WHO_AM_I, &reg) == E_OK, 0);

  return (reg & MPU6050_WHO_AM_I_MASK) >> MPU6050_WHO_AM_I_POS;
}

/* Shared functions ========================================================= */
error_t mpu6050_init(mpu6050_t * mpu6050, mpu6050_cfg_t * cfg) {
  ASSERT_RETURN(mpu6050 && cfg, E_NULL);

  mpu6050->i2c = cfg->i2c;

  uint8_t who_am_i = mpu6050_read_who_am_i(mpu6050);

  if (who_am_i != MPU6050_WHO_AM_I) {
    log_error("Invalid value for WHO_AM_I (%x != %x)", who_am_i, MPU6050_WHO_AM_I);
  }

  log_info("Initializing MPU6050...");

  ERROR_CHECK_RETURN(mpu6050_reg_write(mpu6050, MPU6050_REG_PWR_MGMT_1, MPU6050_CLK_SRC_INTERNAL_8MHZ << MPU6050_PWR_MGMT_1_POS_CLKSEL));
  ERROR_CHECK_RETURN(mpu6050_reg_write(mpu6050, MPU6050_REG_GYRO_CONFIG, cfg->gyro << MPU6050_GYRO_CONFIG_POS_FS_SEL));
  ERROR_CHECK_RETURN(mpu6050_reg_write(mpu6050, MPU6050_REG_ACCEL_CONFIG, cfg->accel << MPU6050_ACCEL_CONFIG_POS_AFS_SEL));

  return E_OK;
}

error_t mpu6050_reset(mpu6050_t * mpu6050) {
  ASSERT_RETURN(mpu6050, E_NULL);

  return mpu6050_reg_write(mpu6050, MPU6050_REG_PWR_MGMT_1, MPU6050_PWR_MGMT_1_MASK_DEVICE_RESET);
}

error_t mpu6050_sleep(mpu6050_t * mpu6050) {
  ASSERT_RETURN(mpu6050, E_NULL);

  return mpu6050_reg_write(mpu6050, MPU6050_REG_PWR_MGMT_1, MPU6050_PWR_MGMT_1_MASK_SLEEP);
}

error_t mpu6050_measure(mpu6050_t * mpu6050, mpu6050_measurement_t * data) {
  ASSERT_RETURN(mpu6050 && data, E_NULL);

  uint8_t tmp[2];

  ERROR_CHECK_RETURN(mpu6050_read_burst(mpu6050, MPU6050_REG_ACCEL_XOUT_H, tmp, 2));
  data->accel.x = (int16_t) tmp[0] << 8 | tmp[1];

  ERROR_CHECK_RETURN(mpu6050_read_burst(mpu6050, MPU6050_REG_ACCEL_YOUT_H, tmp, 2));
  data->accel.y = (int16_t) tmp[0] << 8 | tmp[1];

  ERROR_CHECK_RETURN(mpu6050_read_burst(mpu6050, MPU6050_REG_ACCEL_ZOUT_H, tmp, 2));
  data->accel.z = (int16_t) tmp[0] << 8 | tmp[1];

  ERROR_CHECK_RETURN(mpu6050_read_burst(mpu6050, MPU6050_REG_TEMP_OUT_H, tmp, 2));
  data->temp.val = (int16_t) tmp[0] << 8 | tmp[1];

  ERROR_CHECK_RETURN(mpu6050_read_burst(mpu6050, MPU6050_REG_GYRO_XOUT_H, tmp, 2));
  data->gyro.x = (int16_t) tmp[0] << 8 | tmp[1];

  ERROR_CHECK_RETURN(mpu6050_read_burst(mpu6050, MPU6050_REG_GYRO_YOUT_H, tmp, 2));
  data->gyro.y = (int16_t) tmp[0] << 8 | tmp[1];

  ERROR_CHECK_RETURN(mpu6050_read_burst(mpu6050, MPU6050_REG_GYRO_ZOUT_H, tmp, 2));
  data->gyro.z = (int16_t) tmp[0] << 8 | tmp[1];

  return E_OK;
}
