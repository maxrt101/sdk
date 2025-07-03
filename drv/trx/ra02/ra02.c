/** ========================================================================= *
 *
 * @file ra02.h
 * @date 01-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief TRX API Implementation for LoRa RA-02 module based on sx1278
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <string.h>
#include "drv/trx/ra02/ra02.h"
#include "drv/trx/ra02/ra02_regs.h"
#include "drv/trx/trx.h"
#include "time/sleep.h"
#include "log/log.h"
#include "spi/spi.h"

/* Defines ================================================================== */
#define LOG_TAG RA02

/** Internal constants */
#define RA02_MAX_PA           20

#define RA02_RSSI_HF_SUB (-157)
#define RA02_RSSI_LF_SUB (-164)

/** Default internal trx configuration parameters */
#define RA02_DEFAULT_CRC_RATE RA02_CRC_RATE_4_7 /* CRC Rate */
#define RA02_DEFAULT_SF       8                 /* Spreading Factor */
#define RA02_DEFAULT_OCP_MA   120               /* OverCurrentProtection is mA */

/** Initial trx configuration parameters */
#define RA02_INIT_FREQ        433000            /* Initial frequency */
#define RA02_INIT_POWER       17                /* Initial output power */
#define RA02_INIT_BANDWIDTH   125000            /* Initial bandwidth */
#define RA02_INIT_PREAMBLE    10                /* Initial preamble size */

/* Macros =================================================================== */
/* Enums ==================================================================== */
/**
 * RA-02 OpModes
 */
typedef enum {
  RA02_OP_MODE_SLEEP         = 0,
  RA02_OP_MODE_STANDBY       = 1,
  RA02_OP_MODE_TX            = 3,
  RA02_OP_MODE_RX_CONTINUOUS = 5,
  RA02_OP_MODE_RX_SINGLE     = 6,
} ra02_op_mode_t;

/**
 * RA-02 CRC Rate values
 */
typedef enum {
  RA02_CRC_RATE_4_5 = 1,
  RA02_CRC_RATE_4_6 = 2,
  RA02_CRC_RATE_4_7 = 3,
  RA02_CRC_RATE_4_8 = 4,
} ra02_crc_rate_t;

/**
 * RA-02 Power conversion table
 */
typedef enum {
  RA02_POWER_11 = 0xF6,
  RA02_POWER_14 = 0xF9,
  RA02_POWER_17 = 0xFC,
  RA02_POWER_20 = 0xFF,
} ra02_power_t;

/**
 * RA-02 Bandwidth conversion table
 */
typedef enum {
  RA02_BANDWIDTH_7_8_KHZ  = 0,
  RA02_BANDWIDTH_10_4_KHZ = 1,
  RA02_BANDWIDTH_15_6_KHZ = 2,
  RA02_BANDWIDTH_20_8_KHZ = 3,
  RA02_BANDWIDTH_31_2_KHZ = 4,
  RA02_BANDWIDTH_41_7_KHZ = 5,
  RA02_BANDWIDTH_62_5_KHZ = 6,
  RA02_BANDWIDTH_125_KHZ  = 7,
  RA02_BANDWIDTH_250_KHZ  = 8,
  RA02_BANDWIDTH_500_KHZ  = 9,
} ra02_bandwidth_t;

/* Types ==================================================================== */
/* Variables ================================================================ */
/**
 * RA-02 TRX API Function Table
 */
trx_fn_t TRX_RA02_FN = {
    .init = ra02_init,
    .deinit = ra02_deinit,
    .reset = ra02_reset,
    .sleep = ra02_sleep,
    .set_freq = ra02_set_freq,
    .get_power = ra02_get_power,
    .set_power = ra02_set_power,
    .set_sync_word = ra02_set_sync_word,
    .set_baudrate = ra02_set_baudrate,
    .set_bandwidth = ra02_set_bandwidth,
    .set_preamble = ra02_set_preamble,
    .get_rssi = ra02_get_rssi,
    .irq_handler = ra02_irq_handler,
    .send = ra02_send,
    .recv = ra02_recv,
};

/**
 * RA-02 Power mapping table
 */
static util_map_range_table_entry_t ra02_power_mapping_db[] = {
    {.from = 1,  .to = 10, .value = RA02_POWER_11},
    {.from = 11, .to = 13, .value = RA02_POWER_11},
    {.from = 14, .to = 16, .value = RA02_POWER_14},
    {.from = 17, .to = 19, .value = RA02_POWER_17},
    {.from = 20, .to = 99, .value = RA02_POWER_20},
    /* Terminating entry */
    {.from = 0, .to = 0, .value = 0}
};

/**
 * RA-02 Bandwidth mapping table
 */
static util_map_range_table_entry_t ra02_bandwidth_mapping_hz[] = {
    {.from = 1,      .to = 7799,   .value = RA02_BANDWIDTH_7_8_KHZ},
    {.from = 7800,   .to = 10399,  .value = RA02_BANDWIDTH_7_8_KHZ},
    {.from = 10400,  .to = 15599,  .value = RA02_BANDWIDTH_10_4_KHZ},
    {.from = 15600,  .to = 20799,  .value = RA02_BANDWIDTH_15_6_KHZ},
    {.from = 20800,  .to = 31199,  .value = RA02_BANDWIDTH_20_8_KHZ},
    {.from = 31200,  .to = 41699,  .value = RA02_BANDWIDTH_31_2_KHZ},
    {.from = 41700,  .to = 62499,  .value = RA02_BANDWIDTH_41_7_KHZ},
    {.from = 62500,  .to = 124999, .value = RA02_BANDWIDTH_62_5_KHZ},
    {.from = 125000, .to = 249000, .value = RA02_BANDWIDTH_125_KHZ},
    {.from = 250000, .to = 499000, .value = RA02_BANDWIDTH_250_KHZ},
    {.from = 500000, .to = 999999, .value = RA02_BANDWIDTH_500_KHZ},
    /* Terminating entry */
    {.from = 0, .to = 0, .value = 0}
};

/* Private functions ======================================================== */
/**
 * Write value to register using SPI bus
 */
static error_t ra02_write_reg(trx_t * trx, uint8_t reg, uint8_t value) {
  uint8_t data[2] = {reg | 0x80, value};

  ERROR_CHECK_RETURN(spi_select(trx->ra02.spi));
  error_t err = spi_send(trx->ra02.spi, data, sizeof(data));
  ERROR_CHECK_RETURN(spi_unselect(trx->ra02.spi));

  log_debug("ra02_write_reg: %s reg=%02x val=%02x data={%02x, %02x}",
            error2str(err), reg, value, data[0], data[1]);

  return err;
}

/**
 * Read value from register using SPI bus
 */
static error_t ra02_read_reg(trx_t * trx, uint8_t reg, uint8_t * value) {
  uint8_t tx_data[2] = {reg & 0x7F, 0};
  uint8_t rx_data[2] = {0};

  ERROR_CHECK_RETURN(spi_select(trx->ra02.spi));
  error_t err = spi_send_recv(
      trx->ra02.spi, tx_data, 2, rx_data, 2
  );
  ERROR_CHECK_RETURN(spi_unselect(trx->ra02.spi));

  log_debug("ra02_read_reg: %s reg=%02x res={%02x, %02x}", error2str(err), reg, rx_data[0], rx_data[1]);

  if (err == E_OK) {
    *value = rx_data[1];
  }

  return err;
}

/**
 * Write buffer to register using SPI bus
 */
static error_t ra02_write_burst(trx_t * trx, uint8_t addr, uint8_t * buf, size_t size) {
  uint8_t data = addr | 0x80;

  ERROR_CHECK_RETURN(spi_select(trx->ra02.spi));
  ERROR_CHECK_RETURN(spi_send(trx->ra02.spi, &data, sizeof(data)));
  error_t err= spi_send(trx->ra02.spi, buf, size);
  ERROR_CHECK_RETURN(spi_unselect(trx->ra02.spi));

  return err;
}

/**
 * Transitions RA-02 to selected OpMode
 */
static error_t ra02_goto_op_mode(trx_t * trx, ra02_op_mode_t mode) {
  return ra02_write_reg(trx, RA02_REG_OP_MODE, RA02_OP_MODE_LORA_PREFIX | mode);
}

/**
 * Set over current protection
 *
 * @param[in] current Current threshold in mA
 */
static error_t ra02_set_ocp(trx_t * trx, uint8_t current) {
  current = UTIL_CAP(current, 45, 240);

  if (current <= 120) {
    current = (current - 45) / 5;
  } else if (current <= 120) {
    current = (current - 30) / 10;
  }

  return ra02_write_reg(trx, RA02_REG_OCP, current + (1 << 5));
}

/**
 * Set CRC on/off
 */
static error_t ra02_set_crc(trx_t * trx, bool on) {
  uint8_t data;
  ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_LORA_REG_MODEM_CFG_2, &data));
  // TODO: define modem cfg configuration values
  return ra02_write_reg(trx, RA02_LORA_REG_MODEM_CFG_2, (on << 2) | data);
}

/**
 * Set implicit header mode on/off
 */
static error_t ra02_set_implicit_header_mode(trx_t * trx, bool on) {
  uint8_t data;
  ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_LORA_REG_MODEM_CFG_1, &data));
  return ra02_write_reg(trx, RA02_LORA_REG_MODEM_CFG_1, on ? (data | 1) : (data & ~(1)));
}

/**
 * Set RX Symbol timeout
 */
static error_t ra02_set_rx_symbol_timeout(trx_t * trx, uint16_t value) {
  uint8_t data;
  ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_LORA_REG_MODEM_CFG_2, &data));
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_LORA_REG_MODEM_CFG_2, data | ((value >> 8) & 0x3)));
  return ra02_write_reg(trx, RA02_LORA_REG_SYMB_TIMEOUT_LSB, value & 0xFF); /* Set timeout to max */
}

/**
 * Set Spreading Factor
 */
static error_t ra02_set_sf(trx_t * trx, uint8_t sf) {
  uint8_t data;
  sf = UTIL_CAP(sf, 7, 12);
  ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_LORA_REG_MODEM_CFG_2, &data));
  // TODO: define modem cfg configuration values
  return ra02_write_reg(trx, RA02_LORA_REG_MODEM_CFG_2, (sf << 4) | data);
}

/* Shared functions ========================================================= */
error_t trx_ra02_init(trx_t * trx) {
  ASSERT_RETURN(trx, E_NULL);
  memcpy(&trx->fn, &TRX_RA02_FN, sizeof(trx->fn));
  return E_OK;
}

error_t ra02_init(trx_t * trx, trx_cfg_t * cfg) {
  trx->ra02.spi         = cfg->ra02.spi;
  trx->ra02.reset       = cfg->ra02.reset;
  trx->ra02.irq_flags   = 0;

  ra02_reset(trx);

  uint8_t version;
  ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_REG_VERSION, &version));

  log_debug("ra02_init: version=0x%02x", version);

  ASSERT_RETURN(version == RA02_HW_VERSION, E_NORESP);

  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_REG_OP_MODE, RA02_OP_MODE_SLEEP));

  /** Configure TRX */
  ERROR_CHECK_RETURN(ra02_goto_op_mode(trx, RA02_OP_MODE_SLEEP)); /* Transition RA02 to LoRa mode */
  ERROR_CHECK_RETURN(ra02_set_freq(trx, RA02_INIT_FREQ)); /* Set init frequency */
  ERROR_CHECK_RETURN(ra02_set_power(trx, RA02_INIT_POWER)); /* Set init output power */
  ERROR_CHECK_RETURN(ra02_set_ocp(trx, RA02_DEFAULT_OCP_MA)); /* Set OverCurrentProtection */
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_REG_LNA, 0x23)); /* Set LNA */
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_LORA_REG_MODEM_CFG_2, 0)); /* Reset Modem Cfg 2 */
  ERROR_CHECK_RETURN(ra02_set_implicit_header_mode(trx, false)); /* Set implicit header mode */
  ERROR_CHECK_RETURN(ra02_set_rx_symbol_timeout(trx, 0x2FF)); /* Set RX Symbol Timeout */
  ERROR_CHECK_RETURN(ra02_set_sf(trx, RA02_DEFAULT_SF)); /* Set Spreading Factor */
  ERROR_CHECK_RETURN(ra02_set_bandwidth(trx, RA02_INIT_BANDWIDTH)); /* Set init bandwidth */
  ERROR_CHECK_RETURN(ra02_set_preamble(trx, RA02_INIT_PREAMBLE)); /* Set init preamble */

  return ra02_goto_op_mode(trx, RA02_OP_MODE_STANDBY);
}

error_t ra02_deinit(trx_t * trx) {
  return E_OK;
}

error_t ra02_reset(trx_t * trx) {
  gpio_clear(trx->ra02.reset);
  // TODO: Check delays
  sleep_ms(10);
  gpio_set(trx->ra02.reset);
  // TODO: Check delays
  sleep_ms(5);
  return E_OK;
}

error_t ra02_sleep(trx_t * trx) {
  return ra02_goto_op_mode(trx, RA02_OP_MODE_SLEEP);
}

error_t ra02_set_freq(trx_t * trx, uint32_t khz) {
  uint32_t freq = ((khz/1000) * 524288) >> 5;

  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_REG_FRF_MSB, freq >> 16));
  // TODO: Check delays
  sleep_ms(5);
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_REG_FRF_MID, freq >> 8));
  // TODO: Check delays
  sleep_ms(5);
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_REG_FRF_LSB, freq));
  // TODO: Check delays
  sleep_ms(5);

  return E_OK;
}

error_t ra02_get_power(trx_t * trx, uint8_t * db) {
  ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_REG_PA_CFG, db));

  UTIL_MAP_RANGE_TABLE_REV(ra02_power_mapping_db, *db, *db);

  return E_OK;
}

error_t ra02_set_power(trx_t * trx, uint8_t db) {
  if (db < 1 || db > RA02_MAX_PA) {
    return E_INVAL;
  }
  UTIL_MAP_RANGE_TABLE(ra02_power_mapping_db, db, db);
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_REG_PA_CFG, db));
  // TODO: Check delays
  sleep_ms(10);
  return E_OK;
}

error_t ra02_set_sync_word(trx_t * trx, uint32_t sync_word) {
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_LORA_REG_SYNC_WORD, sync_word));
  // TODO: Check delays
  sleep_ms(10);
  return E_OK;
}

error_t ra02_set_baudrate(trx_t * trx, uint32_t baudrate) {
  return E_NOTIMPL;
}

error_t ra02_set_bandwidth(trx_t * trx, uint32_t bandwidth) {
  UTIL_MAP_RANGE_TABLE(ra02_bandwidth_mapping_hz, bandwidth, bandwidth);

  uint8_t data;
  ra02_read_reg(trx, RA02_LORA_REG_MODEM_CFG_1, &data);

  data = (bandwidth << 4) | (RA02_DEFAULT_CRC_RATE << 1) | data;
  return ra02_write_reg(trx, RA02_LORA_REG_MODEM_CFG_1, data);
}

error_t ra02_set_preamble(trx_t * trx, uint32_t preamble) {
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_LORA_REG_PREAMBLE_MSB, preamble >> 8));
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_LORA_REG_PREAMBLE_LSB, preamble));
  return E_OK;
}

error_t ra02_get_rssi(trx_t * trx, int16_t * rssi) {
  *rssi = RA02_RSSI_HF_SUB + trx->ra02.last_rssi;
  return E_OK;
}

error_t ra02_irq_handler(trx_t * trx) {
  ra02_read_reg(trx, RA02_LORA_REG_IRQ_FLAGS, &trx->ra02.irq_flags);
  ra02_write_reg(trx, RA02_LORA_REG_IRQ_FLAGS, trx->ra02.irq_flags);

  log_debug("DIO0 IRQ: 0x%02x", trx->ra02.irq_flags);

  return E_OK;
}

error_t ra02_send(trx_t * trx, uint8_t * buf, size_t size) {
  error_t err = E_OK;
  uint8_t data;

  trx->ra02.irq_flags = 0;

  ERROR_CHECK_RETURN(ra02_goto_op_mode(trx, RA02_OP_MODE_STANDBY));

  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_REG_DIO_MAP_1,
                                    RA02_LORA_MAP_DIO_0(RA02_LORA_DIO_0_TX_DONE)));

  ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_LORA_REG_FIFO_TX_BASE_ADDR, &data));
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_LORA_REG_FIFO_ADDR_PTR, data));
  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_LORA_REG_PAYLOAD_LEN, size));
  ERROR_CHECK_RETURN(ra02_write_burst(trx, RA02_REG_FIFO, buf, size));

  ERROR_CHECK_RETURN(ra02_goto_op_mode(trx, RA02_OP_MODE_TX));

  TIMEOUT_CREATE(t, TRX_SEND_IRQ_TIMEOUT);

  while (1) {
    if (timeout_is_expired(&t)) {
      err = E_TIMEOUT;
      break;
    }

    if (trx->ra02.irq_flags & RA02_LORA_IRQ_FLAGS_TX_DONE) {
      break;
    }

    trx_on_waiting(trx);
  }

  ERROR_CHECK_RETURN(ra02_goto_op_mode(trx, RA02_OP_MODE_SLEEP));

  return err;
}

error_t ra02_recv(trx_t * trx, uint8_t * buf, size_t * size, timeout_t * timeout) {
  uint8_t data;

  trx->ra02.irq_flags = 0;

  ERROR_CHECK_RETURN(ra02_goto_op_mode(trx, RA02_OP_MODE_STANDBY));

  ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_REG_DIO_MAP_1,
                                    RA02_LORA_MAP_DIO_0(RA02_LORA_DIO_0_RX_DONE)));

  ERROR_CHECK_RETURN(ra02_goto_op_mode(trx, RA02_OP_MODE_RX_SINGLE));

  while (1) {
    if (timeout && timeout_is_expired(timeout)) {
      ERROR_CHECK_RETURN(ra02_goto_op_mode(trx, RA02_OP_MODE_SLEEP));
      return E_TIMEOUT;
    }

    if (trx->ra02.irq_flags & RA02_LORA_IRQ_FLAGS_VALID_HDR) {
      ra02_read_reg(trx, RA02_LORA_REG_LAST_PKT_RSSI_VAL, &trx->ra02.last_rssi);
    }

    if (trx->ra02.irq_flags & RA02_LORA_IRQ_FLAGS_RX_DONE) {
      ERROR_CHECK_RETURN(ra02_goto_op_mode(trx, RA02_OP_MODE_STANDBY));

      /* Read received size */
      ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_LORA_REG_RX_NB_BYTES, &data));

      *size = data > *size ? *size : data;

      ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_LORA_REG_FIFO_RX_CURRENT_ADDR, &data));
      ERROR_CHECK_RETURN(ra02_write_reg(trx, RA02_LORA_REG_FIFO_ADDR_PTR, data));

      for (size_t i = 0; i < *size; ++i) {
        ERROR_CHECK_RETURN(ra02_read_reg(trx, RA02_REG_FIFO, &buf[i]));
      }

      ERROR_CHECK_RETURN(ra02_goto_op_mode(trx, RA02_OP_MODE_SLEEP));
      return E_OK;
    }

    trx_on_waiting(trx);
  }
}
