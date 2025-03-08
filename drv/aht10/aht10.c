/** ========================================================================= *
 *
 * @file aht10.c
 * @date 06-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "aht10/aht10.h"
#include "error/assertion.h"
#include "util/bits.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG AHT10

#define AHT10_CMD_INIT            0xE1
#define AHT10_CMD_RESET           0xBA
#define AHT10_CMD_TRIGGER_MEASURE 0xAC

#define POW_2_20                 1048576
#define AHT10_TEMP_SCALER        1000

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
static error_t aht10_send(aht10_t * ctx, uint8_t * buffer, size_t size) {
  ASSERT_RETURN(ctx, E_NULL);

  return i2c_send(ctx->i2c, ctx->addr, buffer, size);
}

static error_t aht10_send_init(aht10_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  uint8_t buffer[3] = {
    AHT10_CMD_INIT, 0x08, 0x00
  };

  return aht10_send(ctx, buffer, sizeof(buffer));
}

static error_t aht10_parse_temp(aht10_t * ctx, uint8_t * data, aht10_measurement_t * measurement) {
  ASSERT_RETURN(ctx && data && measurement, E_NULL);

  uint32_t raw_temp =
      ((uint32_t) data[3] & 0x0F) << 16 |
       (uint16_t) data[4]         << 8  |
                  data[5];

  // FIXME: Check with reference values
  measurement->temp.value = (((int32_t) raw_temp * AHT10_TEMP_SCALER / POW_2_20) * 200) / AHT10_TEMP_SCALER - 50;
  // FIXME: Fraction is probably wrong
  measurement->temp.fraction = ((((int32_t) raw_temp * AHT10_TEMP_PRECISION / POW_2_20) * 200) - 50) % AHT10_TEMP_PRECISION;

  log_debug("aht10_parse_temp: raw=0x%x parsed=%d.%d",
    raw_temp, measurement->temp.value, measurement->temp.fraction);

  return E_OK;
}

static error_t aht10_parse_humidity(aht10_t * ctx, uint8_t * data, aht10_measurement_t * measurement) {
  ASSERT_RETURN(ctx && data && measurement, E_NULL);

  uint32_t raw_humidity = (
        (uint32_t) data[1] << 16 |
        (uint16_t) data[2] << 8  |
                   data[3]
    ) >> 4;

  measurement->humidity.value = raw_humidity * 100 / POW_2_20;
  measurement->humidity.fraction = raw_humidity * 100 * AHT10_HUMIDITY_PRECISION / POW_2_20 % AHT10_HUMIDITY_PRECISION;

  log_debug("aht10_parse_humidity: raw=0x%x parsed=%d.%d",
    raw_humidity, measurement->humidity.value, measurement->humidity.fraction);

  return E_OK;
}

/* Shared functions ========================================================= */
error_t aht10_init(aht10_t * ctx, i2c_t * i2c, uint16_t addr) {
  ASSERT_RETURN(ctx && i2c, E_NULL);

  ctx->i2c = i2c;
  ctx->addr = addr;

  return aht10_send_init(ctx);
}

error_t aht10_deinit(aht10_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  return E_NOTIMPL;
}

error_t aht10_reset(aht10_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  uint8_t buffer[1] = {
    AHT10_CMD_RESET
  };

  return aht10_send(ctx, buffer, sizeof(buffer));
}

error_t aht10_measure(aht10_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  uint8_t buffer[3] = {
    AHT10_CMD_TRIGGER_MEASURE, 0x33, 0x00
  };

  return aht10_send(ctx, buffer, sizeof(buffer));
}

error_t aht10_read(aht10_t * ctx, aht10_measurement_t * measurement) {
  ASSERT_RETURN(ctx && measurement, E_NULL);

  uint8_t data[6] = {0};

  ERROR_CHECK_RETURN(i2c_recv(ctx->i2c, ctx->addr, data, sizeof(data)));

#define FH2 "%02x "
#define D(i) data[i]
  log_info(
    "aht10_read: "
    FH2 FH2 FH2 FH2 FH2 FH2,
    D(0), D(1), D(2), D(3), D(4), D(5)
  );
#undef FH2
#undef D

  if ((data[0] & 0x80) == 0) {
    ERROR_CHECK_RETURN(aht10_parse_temp(ctx, data, measurement));
    ERROR_CHECK_RETURN(aht10_parse_humidity(ctx, data, measurement));
    return E_OK;
  }

  return E_CANCELLED;
}
