/** ========================================================================= *
 *
 * @file max3010x_regs.h
 * @date 31-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief MAX3010X Registers & Register Field Masks/Positions
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
/* Defines ================================================================== */
/* MAX30100/MAX30102 Common Registers/Constants */
#define MAX3010X_I2C_ADDR                   0x57

#define MAX30100_PART_ID                    0x11
#define MAX30102_PART_ID                    0x15

#define MAX3010X_REG_REV_ID                 0xEE
#define MAX3010X_REG_PART_ID                0xFF

/* MAX30100/MAX30101 Common Register Field Masks & Positions */
#define MAX3010X_MODE_MASK_MODE             0b00000111
#define MAX3010X_MODE_MASK_RST              0b01000000
#define MAX3010X_MODE_MASK_SHDN             0b10000000

#define MAX3010X_MODE_POS_RST               6
#define MAX3010X_MODE_POS_SHDN              7

#define MAX3010X_SPO2_CFG_MASK_LED_PW       0b00000011
#define MAX3010X_SPO2_CFG_MASK_SR           0b00011100

#define MAX3010X_SPO2_CFG_POS_SR            2


/* MAX30100 Registers */
#define MAX30100_REG_IRQ_STATUS             0x00
#define MAX30100_REG_IRQ_ENABLE             0x01
#define MAX30100_REG_FIFO_WRITE_PTR         0x02
#define MAX30100_REG_FIFO_OVF_COUNTER       0x03
#define MAX30100_REG_FIFO_READ_PTR          0x04
#define MAX30100_REG_FIFO_DATA              0x05
#define MAX30100_REG_MODE_CFG               0x06
#define MAX30100_REG_SPO2_CFG               0x07
#define MAX30100_REG_LED_CFG                0x09
#define MAX30100_REG_TEMP_INT               0x16
#define MAX30100_REG_TEMP_FRAC              0x17

/* MAX30100 IRQ Flags */
#define MAX30100_IRQ_STATUS_PWR_RDY         (1 << 0)
#define MAX30100_IRQ_STATUS_SPO2_RDY        (1 << 4)
#define MAX30100_IRQ_STATUS_HR_RDY          (1 << 5)
#define MAX30100_IRQ_STATUS_TEMP_RDY        (1 << 6)
#define MAX30100_IRQ_STATUS_A_FULL          (1 << 7)

#define MAX30100_IRQ_ENABLE_SPO2_RDY        (1 << 4)
#define MAX30100_IRQ_ENABLE_HR_RDY          (1 << 5)
#define MAX30100_IRQ_ENABLE_TEMP_RDY        (1 << 6)
#define MAX30100_IRQ_ENABLE_A_FULL          (1 << 7)

/* MAX30100 Register Field Masks & Positions */
#define MAX30100_MODE_MASK_TEMP_EN          0b00001000

#define MAX30100_MODE_POS_TEMP_EN           3

#define MAX30100_SPO2_CFG_MASK_HI_RES_EN    0b01000000

#define MAX30100_SPO2_CFG_POS_HI_RES_EN     6

#define MAX30100_LED_CFG_MASK_IR            0b00001111
#define MAX30100_LED_CFG_MASK_RED           0b11110000

#define MAX30100_LED_CFG_POS_RED            4


/* MAX30102 Registers */
#define MAX30102_REG_IRQ_STATUS_1           0x00
#define MAX30102_REG_IRQ_STATUS_2           0x01
#define MAX30102_REG_IRQ_ENABLE_1           0x02
#define MAX30102_REG_IRQ_ENABLE_2           0x03
#define MAX30102_REG_FIFO_WRITE_PTR         0x04
#define MAX30102_REG_FIFO_OVF_COUNTER       0x05
#define MAX30102_REG_FIFO_READ_PTR          0x06
#define MAX30102_REG_FIFO_DATA              0x07
#define MAX30102_REG_FIFO_CFG               0x08
#define MAX30102_REG_MODE_CFG               0x09
#define MAX30102_REG_SPO2_CFG               0x0A
#define MAX30102_REG_LED_PULSE_AMP1         0x0C
#define MAX30102_REG_LED_PULSE_AMP2         0x0D
#define MAX30102_REG_MULTI_LED_CTRL1        0x11
#define MAX30102_REG_MULTI_LED_CTRL2        0x12
#define MAX30102_REG_TEMP_INT               0x1F
#define MAX30102_REG_TEMP_FRAC              0x20
#define MAX30102_REG_TEMP_CFG               0x21

/* MAX30102 IRQ Flags */
#define MAX30102_IRQ_STATUS_1_PWR_RDY       (1 << 0)
#define MAX30102_IRQ_STATUS_1_ALC_OVF       (1 << 5)
#define MAX30102_IRQ_STATUS_1_PPG_RDY       (1 << 6)
#define MAX30102_IRQ_STATUS_1_A_FULL        (1 << 7)
#define MAX30102_IRQ_STATUS_2_DIE_TEMP_RDY  (1 << 1)

#define MAX30102_IRQ_ENABLE_1_ALC_OVF       (1 << 5)
#define MAX30102_IRQ_ENABLE_1_PPG_RDY       (1 << 6)
#define MAX30102_IRQ_ENABLE_1_A_FULL        (1 << 7)
#define MAX30102_IRQ_ENABLE_2_DIE_TEMP_RDY  (1 << 1)

/* MAX30102 Register Field Masks & Positions */
#define MAX30102_FIFO_CFG_MASK_A_FULL       0b00001111
#define MAX30102_FIFO_CFG_MASK_ROLL_OVER_EN 0b00010000
#define MAX30102_FIFO_CFG_MASK_SMP_AVE      0b11100000

#define MAX30102_FIFO_POS_ROLL_OVER_EN      4
#define MAX30102_FIFO_POS_SMP_AVE           5

#define MAX30102_SPO2_CFG_MASK_ADC_RGE      0b01100000

#define MAX30102_SPO2_CFG_POS_ADC_RGE       5

#define MAX30102_MULTI_LED_CTRL1_MASK_SLOT1 0b00000111
#define MAX30102_MULTI_LED_CTRL1_MASK_SLOT2 0b01110000
#define MAX30102_MULTI_LED_CTRL2_MASK_SLOT3 0b00000111
#define MAX30102_MULTI_LED_CTRL2_MASK_SLOT4 0b01110000

#define MAX30102_MULTI_LED_CTRL1_POS_SLOT2  4
#define MAX30102_MULTI_LED_CTRL2_POS_SLOT4  4

#define MAX30102_DIE_TEMP_FRAC_MASK         0b00001111

#define MAX30102_DIE_TEMP_CFG_MASK_EN       0b00000001


/* Macros =================================================================== */
/**
 * Evaluates to `true` if device, that is handled by __max3010x is MAX30100
 *
 * @param __max3010x Pointer to MAX3010X driver (`max3010x_t *`)
 */
#define IS_MAX30100(__max3010x) \
  ((__max3010x)->part_id == MAX30100_PART_ID)

/**
 * Evaluates to `true` if device, that is handled by __max3010x is MAX30102
 *
 * @param __max3010x Pointer to MAX3010X driver (`max3010x_t *`)
 */
#define IS_MAX30102(__max3010x) \
  ((__max3010x)->part_id == MAX30102_PART_ID)

/**
 * Uses macro magic to evaluate register address for commonly named registers
 * based on device type retrieved from driver context
 *
 * Many registers in MAX30100/30102 share the same name, and mostly the
 * same bitmap (but different register address), such registers are
 * named MAX30100_REG_NAME/MAX30102_REG_NAME, so it is possible to use
 * MAX3010X_REG to retrieve register address based on device type
 *
 * @param __max3010x Pointer to MAX3010X driver (`max3010x_t *`)
 * @param __reg Generic register name (e.g. MODE_CFG)
 */
#define MAX3010X_REG(__max3010x, __reg) ( \
    IS_MAX30102(__max3010x)               \
      ? UTIL_CAT(MAX30102_REG_, __reg)    \
      : UTIL_CAT(MAX30100_REG_, __reg)    \
  )

/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */

#ifdef __cplusplus
}
#endif