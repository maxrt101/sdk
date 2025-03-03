/** ========================================================================= *
 *
 * @file uart.h
 * @date 20-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Generic UART HAL API, has no default implementation
 *
 * Board must implement all functions in this file, using uart_t* as void*
 * pointer for device specific UART context/handle
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdint.h>
#include <stddef.h>
#include "error/error.h"
#include "time/timeout.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Generic UART handle
 */
typedef void uart_t;

/**
 * UART config
 */
typedef struct {
  uint8_t uart_no;
  uint32_t baudrate;
  uint8_t parity;
  uint8_t stop_bits;
  uint8_t data_bits;
} uart_cfg_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initialize UART
 *
 * @param[out] uart handle to be initialized
 * @param[in] cfg UART config
 */
error_t uart_init(uart_t ** uart, uart_cfg_t * cfg);

/**
 * Deinitialize UART
 *
 * @param[in] uart handle to be deinitialized
 */
error_t uart_deinit(uart_t * uart);

/**
 * Set UART baudrate
 *
 * @param[in] uart UART handle
 * @param[in] baudrate New baudrate to set
 */
error_t uart_set_baudrate(uart_t * uart, uint32_t baudrate);

/**
 * Reset UART instance (clear FIFO, etc)
 *
 * @param[in] uart UART handle
 */
error_t uart_reset(uart_t * uart);

/**
 * Returns true if UART has received data
 *
 * @param[in] uart UART handle
 */
bool uart_available(uart_t * uart);

/**
 * Send buffer over UART
 *
 * @param[in] uart UART handle
 * @param[in] buf Data buffer
 * @param[in] size Data buffer size
 */
error_t uart_send(uart_t * uart, const uint8_t * buf, size_t size);

/**
 * Receive data over UART
 *
 * @param[in] uart UART handle
 * @param[in] buf Data buffer
 * @param[in] size Data buffer size (receive size)
 */
error_t uart_recv(uart_t * uart, uint8_t * buf, size_t size, timeout_t * timeout);


#ifdef __cplusplus
}
#endif
