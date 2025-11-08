/** ========================================================================= *
 *
 * @file trx.h
 * @date 01-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Generic TRX API, has no default implementation
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
#include "hal/spi/spi.h"
#include "error/error.h"
#include "error/assertion.h"
#include "time/timeout.h"

#if HAS_TRX_SX1278_SUPPORT
#include "drv/trx/sx1278/sx1278.h"
#endif

/* Defines ================================================================== */
/**
 * Maximum packet size in bytes
 */
#define TRX_MAX_PACKET_SIZE 64

/* Macros =================================================================== */
/* Enums ==================================================================== */
typedef enum trx_ioctl_cmd_t {
  /**
   * Reserved value
   */
  TRX_IOCTL_CMD_RESERVED_0 = 0,

  /**
   * Set RA02 Spreading Factor
   */
  TRX_IOCTL_CMD_SET_SF,
} trx_ioctl_cmd_t;

/* Types ==================================================================== */
/**
 * TRX Config
 *
 * A union of specific driver configs
 */
typedef struct trx_cfg_s {
  union {
#if HAS_TRX_SX1278_SUPPORT
    sx1278_trx_cfg_t sx1278;
#endif
  };
} trx_cfg_t;

/**
 * TRX Context forward declaration
 */
typedef struct trx_s trx_t;

/**
 * TRX API Function Table
 */
typedef struct trx_fn_s {
  error_t (*init)(trx_t * trx, trx_cfg_t * cfg);
  error_t (*deinit)(trx_t * trx);
  error_t (*reset)(trx_t * trx);
  error_t (*sleep)(trx_t * trx);
  error_t (*set_freq)(trx_t * trx, uint32_t khz);
  error_t (*get_power)(trx_t * trx, uint8_t * db);
  error_t (*set_power)(trx_t * trx, uint8_t db);
  error_t (*set_sync_word)(trx_t * trx, uint32_t sync_word);
  error_t (*set_baudrate)(trx_t * trx, uint32_t baudrate);
  error_t (*set_bandwidth)(trx_t * trx, uint32_t bandwidth);
  error_t (*set_preamble)(trx_t * trx, uint32_t preamble);
  error_t (*get_rssi)(trx_t * trx, int16_t * rssi);
  error_t (*irq_handler)(trx_t * trx);
  error_t (*send)(trx_t * trx, uint8_t * buf, size_t size);
  error_t (*recv)(trx_t * trx, uint8_t * buf, size_t * size, timeout_t * timeout);
  error_t (*ioctl)(trx_t * trx, int cmd, va_list args);
} trx_fn_t;

/**
 * TRX Context
 *
 * Contains API function table, and a union of specific driver contexts
 */
struct trx_s {
  trx_fn_t fn;
  union {
#if HAS_TRX_SX1278_SUPPORT
    sx1278_trx_t sx1278;
#endif
  };
};

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes trx
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx TRX API handle
 * @param[in] cfg TRX config (implementation defined)
 */
__STATIC_FORCEINLINE error_t trx_init(trx_t * trx, trx_cfg_t * cfg) {
  ASSERT_RETURN(trx && cfg, E_NULL);
  return trx->fn.init(trx, cfg);
}

/**
 * Deinitializes trx
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx TRX API handle
 */
__STATIC_FORCEINLINE error_t trx_deinit(trx_t * trx) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.deinit(trx);
}

/**
 * Resets trx
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx TRX API handle
 */
__STATIC_FORCEINLINE error_t trx_reset(trx_t * trx) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.reset(trx);
}

/**
 * Move trx to sleep mode
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx TRX API handle
 */
__STATIC_FORCEINLINE error_t trx_sleep(trx_t * trx) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.sleep(trx);
}

/**
 * Sets trx frequency
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx TRX API handle
 * @param[in] khz Frequency in kilohertz
 */
__STATIC_FORCEINLINE error_t trx_set_freq(trx_t * trx, uint32_t khz) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.set_freq(trx, khz);
}

/**
 * Gets output power
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx TRX API handle
 * @param[in] db  Pointer to result
 */
__STATIC_FORCEINLINE error_t trx_get_power(trx_t * trx, uint8_t * db) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.get_power(trx, db);
}

/**
 * Sets output power
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx TRX API handle
 * @param[in] db  Power in decibels
 */
__STATIC_FORCEINLINE error_t trx_set_power(trx_t * trx, uint8_t db) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.set_power(trx, db);
}

/**
 * Sets synchro word
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx       TRX API handle
 * @param[in] sync_word Synchro word
 */
__STATIC_FORCEINLINE error_t trx_set_sync_word(trx_t * trx, uint32_t sync_word) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.set_sync_word(trx, sync_word);
}

/**
 * Sets baud rate
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx      TRX API handle
 * @param[in] baudrate Baudrate
 */
__STATIC_FORCEINLINE error_t trx_set_baudrate(trx_t * trx, uint32_t baudrate) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.set_baudrate(trx, baudrate);
}

/**
 * Sets bandwidth
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx       TRX API handle
 * @param[in] bandwidth Bandwidth
 */
__STATIC_FORCEINLINE error_t trx_set_bandwidth(trx_t * trx, uint32_t bandwidth) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.set_bandwidth(trx, bandwidth);
}

/**
 * Sets preamble size
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx       TRX API handle
 * @param[in] bandwidth Preamble size in bits
 */
__STATIC_FORCEINLINE error_t trx_set_preamble(trx_t * trx, uint32_t preamble) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.set_preamble(trx, preamble);
}

/**
 * Gets packet rssi
 *
 * @note trx->fn must be filled
 *
 * @param[in]  trx  TRX API handle
 * @param[out] rssi RSSI value measured by the module
 */
__STATIC_FORCEINLINE error_t trx_get_rssi(trx_t * trx, int16_t * rssi) {
  ASSERT_RETURN(trx && rssi, E_NULL);
  return trx->fn.get_rssi(trx, rssi);
}

/**
 * Handler IRQ from RF module
 *
 * @note trx->fn must be filled
 * @note Must be called in BSP EXTI IRQ handler
 *
 * @param[in] trx TRX API handle
 */
__STATIC_FORCEINLINE error_t trx_irq_handler(trx_t * trx) {
  ASSERT_RETURN(trx, E_NULL);
  return trx->fn.irq_handler(trx);
}

/**
 * Sends raw data by trx
 *
 * @note trx->fn must be filled
 *
 * @param[in] trx TRX API handle
 * @param[in] buf Buffer to send
 * @param[in] size Buffer size
 */
__STATIC_FORCEINLINE error_t trx_send(trx_t * trx, uint8_t * buf, size_t size) {
  ASSERT_RETURN(trx && buf, E_NULL);
  return trx->fn.send(trx, buf, size);
}

/**
 * Receives data by trx
 *
 * @note trx->fn must be filled
 *
 * @param[in]     trx TRX API handle
 * @param[out]    buf Buffer to send
 * @param[in/out] size Takes buffer size, write actual received size
 * @param[in]     timeout Receive timeout, can be NULL
 */
__STATIC_FORCEINLINE error_t trx_recv(trx_t * trx, uint8_t * buf, size_t * size, timeout_t * timeout) {
  ASSERT_RETURN(trx && buf && size, E_NULL);
  return trx->fn.recv(trx, buf, size, timeout);
}

/**
 * Receives data by trx
 *
 * @note trx->fn must be filled
 *
 * @param[in]     trx TRX API handle
 * @param[in]     cmd IOCTL command, @ref trx_ioctl_cmd_t
 * @param[in/out] ... Var Args
 */
static inline error_t trx_ioctl(trx_t * trx, int cmd, ...) {
  ASSERT_RETURN(trx, E_NULL);
  va_list args;
  va_start(args, cmd);
  error_t err = trx->fn.ioctl(trx, cmd, args);
  va_end(args);
  return err;
}

/**
 * Callback to be called on waiting for some event
 *
 * @param[in] trx TRX API handle
 */
void trx_on_waiting(trx_t * trx);

#ifdef __cplusplus
}
#endif