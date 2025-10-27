/** ========================================================================= *
 *
 * @file sx1278.h
 * @date 01-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief TRX API Implementation for LoRa RA-02 module based on sx1278
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include "hal/spi/spi.h"
#include "hal/gpio/gpio.h"
#include "error/error.h"
#include "time/timeout.h"
#include "queue/queue.h"

/* Defines ================================================================== */
/**
 * Timeout in ms for TX_DONE flag to get up after TX was initiated
 */
#ifndef TRX_SEND_IRQ_TIMEOUT
#define TRX_SEND_IRQ_TIMEOUT 500
#endif

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
typedef struct trx_s trx_t;
typedef struct trx_fn_s trx_fn_t;
typedef struct trx_cfg_s trx_cfg_t;

/**
 * RA-02 TRX driver config
 */
typedef struct {
  spi_t * spi;
  gpio_t reset;
} sx1278_trx_cfg_t;

/**
 * RA-02 TRX driver context
 */
typedef struct {
  spi_t * spi;
  gpio_t reset;
  uint8_t irq_flags;
  int8_t last_rssi;
} sx1278_trx_t;

/* Variables ================================================================ */
/**
 * RA-02 TRX API Function Table
 */
extern trx_fn_t TRX_sx1278_FN;

/* Shared functions ========================================================= */
/**
 * Initializes trx struct for use with this driver
 *
 * @param[in] trx TRX API handle
 */
error_t trx_sx1278_init(trx_t * trx);

/** RA-02 TRX API implementation */
error_t sx1278_init(trx_t * trx, trx_cfg_t * cfg);
error_t sx1278_deinit(trx_t * trx);
error_t sx1278_reset(trx_t * trx);
error_t sx1278_sleep(trx_t * trx);
error_t sx1278_set_freq(trx_t * trx, uint32_t khz);
error_t sx1278_get_power(trx_t * trx, uint8_t * db);
error_t sx1278_set_power(trx_t * trx, uint8_t db);
error_t sx1278_set_sync_word(trx_t * trx, uint32_t sync_word);
error_t sx1278_set_baudrate(trx_t * trx, uint32_t baudrate);
error_t sx1278_set_bandwidth(trx_t * trx, uint32_t bandwidth);
error_t sx1278_set_preamble(trx_t * trx, uint32_t preamble);
error_t sx1278_get_rssi(trx_t * trx, int16_t * rssi);
error_t sx1278_irq_handler(trx_t * trx);
error_t sx1278_send(trx_t * trx, uint8_t * buf, size_t size);
error_t sx1278_recv(trx_t * trx, uint8_t * buf, size_t * size, timeout_t * timeout);
error_t sx1278_ioctl(trx_t * trx, int cmd, va_list args);

#ifdef __cplusplus
}
#endif