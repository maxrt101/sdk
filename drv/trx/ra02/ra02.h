/** ========================================================================= *
 *
 * @file ra02.h
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

/**
 * Queue size for async recv context
 */
#ifndef TRX_QUEUE_SIZE
#define TRX_QUEUE_SIZE 5
#endif

/**
 * Queue size for async recv context
 */
#ifndef TRX_ASYNC_TIMEOUT_MS
#define TRX_ASYNC_TIMEOUT_MS 500
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
} ra02_trx_cfg_t;

/**
 * Container for incoming data, used in async_recv
 */
typedef struct {
  bool used;
  uint8_t size;
  uint8_t data[];
} ra02_trx_packet_t;

/**
 * RA-02 TRX Async Recv Context, used by async_recv
 */
typedef struct ra02_trx_async_ctx_s {
  /**
   * Async Recv State
   */
  enum {
    RA02_ASYNC_STATE_NONE = 0,
    RA02_ASYNC_STATE_INIT,
    RA02_ASYNC_STATE_RECV,
  } state;

  /**
   * Timeout for single RX operation, automatically restarts
   */
  timeout_t timeout;

  /**
   * Storage for received packet data
   */
  ra02_trx_packet_t packets[TRX_QUEUE_SIZE];

  /**
   * Queue handle and element storage
   */
  struct {
    queue_element_t elements[TRX_QUEUE_SIZE];
    queue_t handle;
  } queue;
} ra02_trx_async_ctx_t;

/**
 * RA-02 TRX driver context
 */
typedef struct {
  spi_t * spi;
  gpio_t reset;
  uint8_t irq_flags;
  ra02_trx_async_ctx_t async;
} ra02_trx_t;

/* Variables ================================================================ */
/**
 * RA-02 TRX API Function Table
 */
extern trx_fn_t TRX_RA02_FN;

/* Shared functions ========================================================= */
/**
 * Initializes trx struct for use with this driver
 *
 * @param[in] trx TRX API handle
 */
error_t trx_ra02_init(trx_t * trx);

/** RA-02 TRX API implementation */
error_t ra02_init(trx_t * trx, trx_cfg_t * cfg);
error_t ra02_deinit(trx_t * trx);
error_t ra02_reset(trx_t * trx);
error_t ra02_sleep(trx_t * trx);
error_t ra02_set_freq(trx_t * trx, uint32_t khz);
error_t ra02_set_power(trx_t * trx, uint8_t db);
error_t ra02_set_sync_word(trx_t * trx, uint32_t sync_word);
error_t ra02_set_baudrate(trx_t * trx, uint32_t baudrate);
error_t ra02_set_bandwidth(trx_t * trx, uint32_t bandwidth);
error_t ra02_set_preamble(trx_t * trx, uint32_t preamble);
error_t ra02_get_rssi(trx_t * trx, uint8_t * rssi);
error_t ra02_irq_handler(trx_t * trx);
error_t ra02_send(trx_t * trx, uint8_t * buf, size_t size);
error_t ra02_recv(trx_t * trx, uint8_t * buf, size_t * size, timeout_t * timeout);
error_t ra02_async_recv(trx_t * trx);
error_t ra02_async_recv_stop(trx_t * trx);
error_t ra02_async_get_pkt(trx_t * trx, uint8_t * buf, size_t * size);

#ifdef __cplusplus
}
#endif