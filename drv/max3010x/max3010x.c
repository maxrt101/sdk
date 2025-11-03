/** ========================================================================= *
 *
 * @file max3010x.c
 * @date 31-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "drv/max3010x/max3010x.h"
#include "drv/max3010x/max3010x_regs.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG max3010x

/** Will log I2C read/write calls if enabled */
#ifndef MAX3010_VERBOSE
#define MAX3010_VERBOSE 0
#endif

/** Helper for retrieving current_to_min_ir_adc_voltage_table size */
#define CURRENT_TO_ADC_VOLTAGE_TABLE_SIZE \
  (sizeof(current_to_min_ir_adc_voltage_table)/sizeof(current_to_min_ir_adc_voltage_table[0]))

/* Macros =================================================================== */
/**
 * Shorthand to reading a generic register
 *
 * @param __max3010x Driver context
 * @param __reg Register name.
 *              Must be without *_REG_ prefix.
 *              Register in question must have defines for MAX30100_REG_<NAME> & MAX30102_REG_<NAME>
 * @param __out Where to put the result. Should be a `uint8_t*`
 */
#define READ_REG_GENERIC(__max3010x, __reg, __out) \
  ERROR_CHECK_RETURN(max3010x_reg_read(__max3010x, MAX3010X_REG(__max3010x, __reg), __out))

/**
 * Shorthand to writing to a generic register
 *
 * @param __max3010x Driver context
 * @param __reg Register name.
 *              Must be without *_REG_ prefix.
 *              Register in question must have defines for MAX30100_REG_<NAME> & MAX30102_REG_<NAME>
 * @param __val Register value to write. Should be a uint8_t, as bigger values will get trimmed
 */
#define WRITE_REG_GENERIC(__max3010x, __reg, __val) \
  ERROR_CHECK_RETURN(max3010x_reg_write(__max3010x, MAX3010X_REG(__max3010x, __reg), __val))

/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/**
 * Table that hold IR LED current & corresponding minimal ADC value for
 * valid readings
 */
static struct {
  uint16_t ma;
  uint16_t adc;
} current_to_min_ir_adc_voltage_table[] = {
  {50, 63000},
  {43, 63000},
  {30, 63000},
  {27, 50000},
  {24, 49000},
  {21, 40000},
  {17, 38000},
  {14, 29000},
  {11, 18000},
  {8,  13000},
  {4,  6000},
};

/* Private functions ======================================================== */
/**
 * Write to a register
 *
 * @note Generic, but MAX3010X_REG() may need to be used for generic register
 *       address handling
 *
 * @param max3010x Driver context
 * @param reg Register address
 * @param val Value to write
 */
static error_t max3010x_reg_write(max3010x_t * max3010x, uint8_t reg, uint8_t val) {
#if MAX3010_VERBOSE
  log_debug("write %02x %02x", reg, val);
#endif

  return i2c_send(max3010x->i2c, MAX3010X_I2C_ADDR, (uint8_t[]){reg, val}, 2);
}

/**
 * Read from a register
 *
 * @note Generic, but MAX3010X_REG() may need to be used for generic register
 *       address handling
 *
 * @param max3010x Driver context
 * @param reg Register address
 * @param val Value to put the result in
 */
static error_t max3010x_reg_read(max3010x_t * max3010x, uint8_t reg, uint8_t * val) {
  ERROR_CHECK_RETURN(i2c_send(max3010x->i2c, MAX3010X_I2C_ADDR, &reg, 1));
  ERROR_CHECK_RETURN(i2c_recv(max3010x->i2c, MAX3010X_I2C_ADDR, val, 1));

#if MAX3010_VERBOSE
  log_debug("read %02x %02x", reg, *val);
#endif

  return E_OK;
}

/**
 * Read from FIFO
 *
 * @note Generic
 *
 * @param max3010x Driver context
 * @param data Buffer to write to
 * @param size Size in bytes
 */
static error_t max3010x_read_fifo(max3010x_t * max3010x, uint8_t * data, uint8_t size) {
  ERROR_CHECK_RETURN(i2c_send(max3010x->i2c, MAX3010X_I2C_ADDR, &(uint8_t){MAX3010X_REG(max3010x, FIFO_DATA)}, 1));
  ERROR_CHECK_RETURN(i2c_recv(max3010x->i2c, MAX3010X_I2C_ADDR, data, size));
  return E_OK;
}

/**
 * Clear FIFO
 *
 * @note Generic
 *
 * @param max3010x Driver context
 */
static error_t max3010x_clear_fifo(max3010x_t * max3010x) {
  WRITE_REG_GENERIC(max3010x, FIFO_WRITE_PTR, 0);
  WRITE_REG_GENERIC(max3010x, FIFO_READ_PTR, 0);
  WRITE_REG_GENERIC(max3010x, FIFO_OVF_COUNTER, 0);

  return E_OK;
}

/* Shared functions ========================================================= */
error_t max3010x_init(max3010x_t * max3010x, max3010x_cfg_t * cfg) {
  ASSERT_RETURN(max3010x && cfg, E_NULL);

  max3010x->i2c = cfg->i2c;
  max3010x->irq_flags_1 = 0;
  max3010x->irq_flags_2 = 0;

  ERROR_CHECK_RETURN(max3010x_reset(max3010x));

  ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX3010X_REG_REV_ID, &max3010x->rev_id));
  ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX3010X_REG_PART_ID, &max3010x->part_id));

  log_info("MAX3010X: Part ID: 0x%x; Revision ID: 0x%x", max3010x->part_id, max3010x->rev_id);

  // Detect MAX30100/MAX30102 by part ID
  if (IS_MAX30102(max3010x)) {
    log_info("MAX30102 Detected");
  } else if (IS_MAX30100(max3010x)) {
    log_info("MAX30100 Detected");
  } else {
    log_error("Unknown part ID");
    max3010x->i2c = NULL;
    return E_NOTIMPL;
  }

  ERROR_CHECK_RETURN(max3010x_clear_fifo(max3010x));
  ERROR_CHECK_RETURN(max30102_configure_fifo(max3010x, MAX30102_SAMPLE_AVG_8, 7, true)); // TODO: Allow configuring FIFO?

  // Set values from config

  ERROR_CHECK_RETURN(max3010x_set_pulse_width(max3010x, IS_MAX30102(max3010x) ? cfg->pulse_width.max30102 : cfg->pulse_width.max30100));
  ERROR_CHECK_RETURN(max3010x_set_sample_rate(max3010x, IS_MAX30102(max3010x) ? cfg->sample_rate.max30102 : cfg->sample_rate.max30100));
  ERROR_CHECK_RETURN(max30102_set_adc_range(max3010x, cfg->adc_range.max30102));
  ERROR_CHECK_RETURN(max3010x_set_ir_led_current(max3010x, cfg->current.ir));
  ERROR_CHECK_RETURN(max3010x_set_red_led_current(max3010x, cfg->current.red));

  ERROR_CHECK_RETURN(max3010x_set_mode(max3010x, cfg->mode));

  uint8_t reg;

  // Turn on A_FULL (almost full) IRQ
  if (IS_MAX30102(max3010x)) {
    ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX30102_REG_IRQ_ENABLE_1, &reg));
    ERROR_CHECK_RETURN(max3010x_reg_write(max3010x, MAX30102_REG_IRQ_ENABLE_1, reg | MAX30102_IRQ_ENABLE_1_A_FULL));

    // TODO: Temperature
    // ERROR_CHECK_RETURN(max3010x_reg_write(max3010x, MAX30102_REG_TEMP_CFG, MAX30102_DIE_TEMP_CFG_MASK_EN));
    //
    // ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX30102_REG_IRQ_ENABLE_2, &reg));
    // ERROR_CHECK_RETURN(max3010x_reg_write(max3010x, MAX30102_REG_IRQ_ENABLE_2, reg | MAX30102_IRQ_ENABLE_2_DIE_TEMP_RDY));
  } else {
    ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX30100_REG_IRQ_ENABLE, &reg));
    ERROR_CHECK_RETURN(max3010x_reg_write(max3010x, MAX30100_REG_IRQ_ENABLE, reg | MAX30100_IRQ_ENABLE_A_FULL));
  }

  return E_OK;
}

error_t max3010x_reset(max3010x_t * max3010x) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);

  return max3010x_reg_write(max3010x, MAX3010X_REG(max3010x, MODE_CFG), 1 << MAX3010X_MODE_POS_RST);
}

error_t max3010x_shutdown(max3010x_t * max3010x) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);

  return max3010x_reg_write(max3010x, MAX3010X_REG(max3010x, MODE_CFG), 1 << MAX3010X_MODE_POS_SHDN);
}

error_t max3010x_set_mode(max3010x_t * max3010x, max3010x_mode_t mode) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);

  uint8_t reg = 0;

  READ_REG_GENERIC(max3010x, MODE_CFG, &reg);
  WRITE_REG_GENERIC(max3010x, MODE_CFG, (reg & ~MAX3010X_MODE_MASK_MODE) | mode);

  return max3010x_clear_fifo(max3010x);
}

error_t max3010x_set_sample_rate(max3010x_t * max3010x, uint8_t rate) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);

  uint8_t reg;

  READ_REG_GENERIC(max3010x, SPO2_CFG, &reg);
  WRITE_REG_GENERIC(max3010x, SPO2_CFG, (reg & ~MAX3010X_SPO2_CFG_MASK_SR) | (rate << MAX3010X_SPO2_CFG_POS_SR));

  return E_OK;
}

error_t max3010x_set_pulse_width(max3010x_t * max3010x, uint8_t pw) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);

  uint8_t reg;

  READ_REG_GENERIC(max3010x, SPO2_CFG, &reg);
  WRITE_REG_GENERIC(max3010x, SPO2_CFG, (reg & ~MAX3010X_SPO2_CFG_MASK_LED_PW) | pw);

  return E_OK;
}

error_t max30102_set_adc_range(max3010x_t * max3010x, max30102_adc_range_t rng) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);
  ASSERT_RETURN(IS_MAX30102(max3010x), E_OK);

  uint8_t reg;

  READ_REG_GENERIC(max3010x, MODE_CFG, &reg);
  WRITE_REG_GENERIC(max3010x, MODE_CFG, (reg & ~MAX30102_SPO2_CFG_MASK_ADC_RGE) | (rng << MAX30102_SPO2_CFG_POS_ADC_RGE));

  return E_OK;
}

error_t max30102_configure_fifo(max3010x_t * max3010x, max30102_sample_avg_t avg, uint8_t a_full_cnt, bool rollover) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);
  ASSERT_RETURN(IS_MAX30102(max3010x), E_OK);

  uint8_t config = (avg << MAX30102_FIFO_POS_SMP_AVE) | (rollover << MAX30102_FIFO_POS_ROLL_OVER_EN) | a_full_cnt;

  return max3010x_reg_write(max3010x, MAX30102_REG_FIFO_CFG, config);
}

error_t max3010x_set_ir_led_current(max3010x_t * max3010x, uint8_t ma) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);

  max3010x->current.ir = ma;

  if (IS_MAX30102(max3010x)) {
    ASSERT_RETURN(ma <= 51, E_INVAL);

    return max3010x_reg_write(max3010x, MAX30102_REG_LED_PULSE_AMP1, ma * 5);
  } else {
    ASSERT_RETURN(ma <= 50, E_INVAL);

    uint8_t reg;
    ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX30100_REG_LED_CFG, &reg));

    return max3010x_reg_write(max3010x, MAX30100_REG_LED_CFG, reg | (ma / 3 - (ma > 20 ? 1 : 0)));
  }
}

error_t max3010x_set_red_led_current(max3010x_t * max3010x, uint8_t ma) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);

  max3010x->current.red = ma;

  if (IS_MAX30102(max3010x)) {
    ASSERT_RETURN(ma <= 51, E_INVAL);

    return max3010x_reg_write(max3010x, MAX30102_REG_LED_PULSE_AMP2, ma * 5);
  } else {
    ASSERT_RETURN(ma <= 50, E_INVAL);

    uint8_t reg;
    ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX30100_REG_LED_CFG, &reg));

    return max3010x_reg_write(max3010x, MAX30100_REG_LED_CFG, reg | ((ma / 3 - (ma > 20 ? 1 : 0)) << MAX30100_LED_CFG_POS_RED));
  }
}

error_t max3010x_read_temp(max3010x_t * max3010x, max3010x_temp_t * temp) {
  ASSERT_RETURN(max3010x && temp, E_NULL);

  READ_REG_GENERIC(max3010x, TEMP_INT, &temp->value);
  READ_REG_GENERIC(max3010x, TEMP_FRAC, &temp->frac);

  return E_OK;
}

error_t max3010x_read_samples(max3010x_t * max3010x, max3010x_sample_t * samples, size_t * size) {
  ASSERT_RETURN(max3010x && max3010x->i2c && samples && size, E_NULL);
  ASSERT_RETURN(*size, E_UNDERFLOW);

  uint8_t write_ptr = 0, read_ptr = 0;;
  READ_REG_GENERIC(max3010x, FIFO_WRITE_PTR, &write_ptr);
  READ_REG_GENERIC(max3010x, FIFO_READ_PTR, &read_ptr);

  // Calculate count of available values
  int8_t sample_count = (int8_t) write_ptr - (int8_t) read_ptr;

  // Consider wrap-around
  if (sample_count < 1) {
    // MAX30100's FIFO is 16 values deep, while MAX30102 - 32
    sample_count += IS_MAX30102(max3010x) ? 32 : 16;
  }

  uint8_t saved = 0;

  for (uint8_t i = 0; i < sample_count; ++i) {
    uint8_t raw[6];

    if (IS_MAX30102(max3010x)) {
      // MAX30102 has 3 bytes per sample - 6 bytes per FIFO frame
      ERROR_CHECK_RETURN(max3010x_read_fifo(max3010x, raw, 6));

      samples[i].ir = (
          (uint32_t) raw[1] << 16 |
          (uint16_t) raw[2] << 8  |
                     raw[3]
        ) & 0x3ffff;

        samples[i].red = (
          (uint32_t) raw[3] << 16 |
          (uint16_t) raw[4] << 8  |
                     raw[5]
        ) & 0x3ffff;
    } else {
      // MAX30102 has 2 bytes per sample - 5 bytes per FIFO frame
      ERROR_CHECK_RETURN(max3010x_read_fifo(max3010x, raw, 4));

      samples[i].ir  = (uint16_t) raw[0] << 8 | raw[1];
      samples[i].red = (uint16_t) raw[2] << 8 | raw[3];
    }

    saved++;

    if (saved >= *size) {
      break;
    }
  }

  *size = saved;

  return E_OK;
}

error_t max3010x_poll_irq_flags(max3010x_t * max3010x) {
  ASSERT_RETURN(max3010x && max3010x->i2c, E_NULL);

  if (IS_MAX30102(max3010x)) {
    ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX30102_REG_IRQ_STATUS_1, &max3010x->irq_flags_1));
    ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX30102_REG_IRQ_STATUS_2, &max3010x->irq_flags_2));
  } else {
    ERROR_CHECK_RETURN(max3010x_reg_read(max3010x, MAX30100_REG_IRQ_STATUS, &max3010x->irq_flags_1));
  }

  return E_OK;
}

max3010x_status_t max3010x_process(max3010x_t * max3010x) {
  ASSERT_RETURN(max3010x && max3010x->i2c, MAX3010X_STATUS_IDLE);

  max3010x_status_t status = MAX3010X_STATUS_IDLE;

  if (IS_MAX30102(max3010x)) {
    if (max3010x->irq_flags_1 & MAX30102_IRQ_STATUS_1_A_FULL) {
      status = MAX3010X_STATUS_SAMPLES_READY;
    }

    if (max3010x->irq_flags_2 & MAX30102_IRQ_STATUS_2_DIE_TEMP_RDY) {
      if (status == MAX3010X_STATUS_IDLE) {
        status = MAX3010X_STATUS_TEMP_READY;
      }
    }
  } else {
    if (max3010x->irq_flags_1 & MAX30100_IRQ_STATUS_A_FULL) {
      status = MAX3010X_STATUS_SAMPLES_READY;
    }

    if (max3010x->irq_flags_1 & MAX30100_IRQ_STATUS_TEMP_RDY) {
      if (status == MAX3010X_STATUS_IDLE) {
        status = MAX3010X_STATUS_TEMP_READY;
      }
    }
  }

  max3010x->irq_flags_1 = 0;
  max3010x->irq_flags_2 = 0;

  return status;
}

uint16_t max3010x_get_min_ir_adc_voltage(max3010x_t * max3010x) {
  ASSERT_RETURN(max3010x, 0);

  uint8_t ma = max3010x->current.ir;

  if (ma >= current_to_min_ir_adc_voltage_table[0].ma) {
    return current_to_min_ir_adc_voltage_table[0].adc; // above max, clamp
  }

  if (ma <= current_to_min_ir_adc_voltage_table[CURRENT_TO_ADC_VOLTAGE_TABLE_SIZE-1].ma) {
    return current_to_min_ir_adc_voltage_table[CURRENT_TO_ADC_VOLTAGE_TABLE_SIZE-1].adc; // below min, clamp
  }

  for (uint8_t i = 0; i < CURRENT_TO_ADC_VOLTAGE_TABLE_SIZE - 1; ++i) {
    if (ma <= current_to_min_ir_adc_voltage_table[i].ma && ma >= current_to_min_ir_adc_voltage_table[i+1].ma) {
      return current_to_min_ir_adc_voltage_table[i].adc;
    }
  }

  return 0;
}
