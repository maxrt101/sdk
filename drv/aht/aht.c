/** ========================================================================= *
 *
 * @file aht.c
 * @date 06-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "aht/aht.h"
#include "error/assertion.h"
#include "util/bits.h"
#include "log/log.h"
#include "time/sleep.h"

/* Defines ================================================================== */
#define LOG_TAG AHT

#define AHT10_CMD_INIT          0xE1
#define AHT20_CMD_INIT          0xBE
#define AHT_CMD_RESET           0xBA
#define AHT_CMD_TRIGGER_MEASURE 0xAC

#define POW_2_20                1048576
#define AHT_TEMP_SCALER         1000

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
static error_t aht_send(aht_t * ctx, uint8_t * buffer, size_t size) {
  ASSERT_RETURN(ctx, E_NULL);

  return i2c_send(ctx->i2c, ctx->addr, buffer, size);
}

static error_t aht_init_detect(aht_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  uint8_t buffer[3] = {
    AHT10_CMD_INIT, 0x08, 0x00
  };

  if (aht_send(ctx, buffer, sizeof(buffer)) == E_OK) {
    ctx->type = AHT_10;
    return E_OK;
  }

  buffer[0] = AHT20_CMD_INIT;

  if (aht_send(ctx, buffer, sizeof(buffer)) == E_OK) {
    ctx->type = AHT_20;
    return E_OK;
  }

  ctx->type = AHT_UNKNOWN;

  return E_FAILED;
}

static error_t aht_parse_temp(aht_t * ctx, uint8_t * data, aht_measurement_t * measurement) {
  ASSERT_RETURN(ctx && data && measurement, E_NULL);

  uint32_t raw_temp =
      ((uint32_t) data[3] & 0x0F) << 16 |
       (uint16_t) data[4]         << 8  |
                  data[5];

  // FIXME: Check with reference values
  measurement->temp.value = (((int32_t) raw_temp * AHT_TEMP_SCALER / POW_2_20) * 200) / AHT_TEMP_SCALER - 50;
  // FIXME: Fraction is probably wrong
  measurement->temp.fraction = ((((int32_t) raw_temp * AHT_TEMP_PRECISION / POW_2_20) * 200) - 50) % AHT_TEMP_PRECISION;

#if AHT_VERBOSE
  log_debug("aht_parse_temp: raw=0x%x parsed=%d.%d",
    raw_temp, measurement->temp.value, measurement->temp.fraction);
#endif

  return E_OK;
}

static error_t aht_parse_humidity(aht_t * ctx, uint8_t * data, aht_measurement_t * measurement) {
  ASSERT_RETURN(ctx && data && measurement, E_NULL);

  uint32_t raw_humidity = (
        (uint32_t) data[1] << 16 |
        (uint16_t) data[2] << 8  |
                   data[3]
    ) >> 4;

  measurement->humidity.value = raw_humidity * 100 / POW_2_20;
  measurement->humidity.fraction = raw_humidity * 100 * AHT_HUMIDITY_PRECISION / POW_2_20 % AHT_HUMIDITY_PRECISION;

#if AHT_VERBOSE
  log_debug("aht_parse_humidity: raw=0x%x parsed=%d.%d",
    raw_humidity, measurement->humidity.value, measurement->humidity.fraction);
#endif

  return E_OK;
}

/* Shared functions ========================================================= */
error_t aht_init(aht_t * ctx, i2c_t * i2c, uint16_t addr) {
  ASSERT_RETURN(ctx && i2c, E_NULL);

  ctx->i2c = i2c;
  ctx->addr = addr;

  ERROR_CHECK_RETURN(aht_reset(ctx));

  return aht_init_detect(ctx);
}

error_t aht_deinit(aht_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  return E_NOTIMPL;
}

error_t aht_reset(aht_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  uint8_t buffer[1] = {
    AHT_CMD_RESET
  };

  error_t err = aht_send(ctx, buffer, sizeof(buffer));

  sleep_ms(20);

  return err;
}

error_t aht_measure(aht_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  uint8_t buffer[3] = {
    AHT_CMD_TRIGGER_MEASURE, 0x33, 0x00
  };

  return aht_send(ctx, buffer, sizeof(buffer));
}

error_t aht_read(aht_t * ctx, aht_measurement_t * measurement) {
  ASSERT_RETURN(ctx && measurement, E_NULL);

  uint8_t data[6] = {0};

  ERROR_CHECK_RETURN(i2c_recv(ctx->i2c, ctx->addr, data, sizeof(data)));

#if AHT_VERBOSE
#define FH2 "%02x "
#define D(i) data[i]
  log_info(
    "aht_read: "
    FH2 FH2 FH2 FH2 FH2 FH2,
    D(0), D(1), D(2), D(3), D(4), D(5)
  );
#undef FH2
#undef D
#endif

  if ((data[0] & 0x80) == 0) {
    ERROR_CHECK_RETURN(aht_parse_temp(ctx, data, measurement));
    ERROR_CHECK_RETURN(aht_parse_humidity(ctx, data, measurement));
    return E_OK;
  }

  return E_CANCELLED;
}

const char * aht_type_to_str(aht_type_t type) {
  switch (type) {
    case AHT_10:
      return "AHT10";

    case AHT_20:
      return "AHT20";

    case AHT_UNKNOWN:
    default:
      return "UNK";
  }
}
