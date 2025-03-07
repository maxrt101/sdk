/** ========================================================================= *
 *
 * @file spi.h
 * @date 01-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Generic SPI HAL API, has no default implementation
 *
 * Board must implement init/deinit/select/unselect/send_recv,
 * using spi_handle_t* as void* pointer for device specific SPI context/handle
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdint.h>
#include <stddef.h>
#include "hal/gpio/gpio.h"
#include "lib/error/error.h"
#include "util/compiler.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Generic SPI handle
 */
typedef void spi_handle_t;

/**
 * SPI Context
 */
typedef struct {
  spi_handle_t * handle;  /** Platform specific handle */
  gpio_t cs;              /** Chip Select pin */
} spi_t;

/**
 * SPI config
 */
typedef struct {
  uint8_t spi_no;         /** SPI Peripheral Number */
  gpio_t cs;              /** Chip Select pin */
} spi_cfg_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initialize SPI
 *
 * @param[out] spi handle to be initialized
 * @param[in] cfg SPI config
 */
error_t spi_init(spi_t * spi, spi_cfg_t * cfg);

/**
 * Deinitialize SPI
 *
 * @param[in] spi handle to be deinitialized
 */
error_t spi_deinit(spi_t * spi);

/**
 * Select SPI device
 *
 * @param[in] spi SPI instance handle
 */
error_t spi_select(spi_t * spi);

/**
 * Unselect SPI device
 *
 * @param[in] spi SPI instance handle
 */
error_t spi_unselect(spi_t * spi);

/**
 * Send and receive data over SPI
 *
 * @param[in] spi SPI handle
 * @param[in] tx_buf Data buffer
 * @param[in] tx_size Data buffer size
 * @param[in] rx_buf Data buffer
 * @param[in] rx_size Data buffer size
 */
error_t spi_send_recv(
    spi_t * spi,
    uint8_t * tx_buf, size_t tx_size,
    uint8_t * rx_buf, size_t rx_size);

/**
 * If you wish to define custom spi_send & spi_recv define
 * USE_SPI_CUSTOM_SEND_RECV to 1 and provide your own implementation
 */
#if USE_SPI_CUSTOM_SEND_RECV
/**
 * Send buffer over SPI
 *
 * @param[in] spi SPI handle
 * @param[in] tx_buf Data buffer
 * @param[in] tx_size Data buffer size
 */
error_t spi_send(spi_t * spi, uint8_t * tx_buf, size_t tx_size);

/**
 * Receive data over SPI
 *
 * @param[in] spi SPI handle
 * @param[in] rx_buf Data buffer
 * @param[in] rx_size Data buffer size
 */
error_t spi_recv(spi_t * spi, uint8_t * rx_buf, size_t rx_size);
#else
/**
 * Send buffer over SPI
 *
 * @note Default spi_send implementation (calls spi_send_recv)
 *
 * @param[in] spi SPI handle
 * @param[in] tx_buf Data buffer
 * @param[in] tx_size Data buffer size
 */
__STATIC_FORCEINLINE error_t spi_send(spi_t * spi, uint8_t * tx_buf, size_t tx_size) {
  return spi_send_recv(spi, tx_buf, tx_size, NULL, 0);
}

/**
 * Receive data over SPI
 *
 * @note Default spi_recv implementation (calls spi_send_recv)
 *
 * @param[in] spi SPI handle
 * @param[in] rx_buf Data buffer
 * @param[in] rx_size Data buffer size
 */
__STATIC_FORCEINLINE error_t spi_recv(spi_t * spi, uint8_t * rx_buf, size_t rx_size) {
  return spi_send_recv(spi, NULL, 0, rx_buf, rx_size);
}
#endif

#ifdef __cplusplus
}
#endif