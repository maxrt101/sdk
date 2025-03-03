/** ========================================================================= *
 *
 * @file onewire.h
 * @date 24-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Generic OneWire HAL API
 *
 * Needs 3 functions to operate: set_baudrate, send, recv
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "hal/gpio/gpio.h"
#include "hal/uart/uart.h"

/* Defines ================================================================== */
#define OW_CMD_SKIP_ROM   0xCC
#define OW_CMD_READ_ROM   0x33
#define OW_CMD_SEARCH_ROM 0xF0
#define OW_CMD_MATCH_ROM  0x55

#define OW_ROM_SIZE       8
#define OW_ID_SIZE        6

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Sets transport baudrate
 *
 * @note void* is to be used as context (onewire_t::trsp.ctx will be passed)
 */
typedef void (*onewire_trsp_set_baudrate_t)(void *, uint32_t);

/**
 * Sends byte over transfer
 *
 * @note void* is to be used as context (onewire_t::trsp.ctx will be passed)
 */
typedef void (*onewire_trsp_send_t)(void *, uint8_t);

/**
 * Receives byte over transfer
 *
 * @note void* is to be used as context (onewire_t::trsp.ctx will be passed)
 */
typedef uint8_t (*onewire_trsp_recv_t)(void *);

/**
 * ROM Contents of onewire device
 */
typedef struct {
  union {
    uint8_t buffer[OW_ROM_SIZE];
    struct {
      uint8_t family_code;
      uint8_t id[OW_ID_SIZE];
      uint8_t crc;
    };
  };
} onewire_rom_t;

/**
 * OneWire device
 */
typedef struct {
  onewire_rom_t rom;
} onewire_device_t;

/**
 * OneWire driver context
 */
typedef struct {
  struct {
    void * ctx;
    onewire_trsp_set_baudrate_t set_baudrate;
    onewire_trsp_send_t send;
    onewire_trsp_recv_t recv;
  } trsp;
} onewire_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes OneWire interface driver
 *
 * @param[in] ctx OneWire Context
 */
error_t onewire_init(onewire_t * ctx);

/**
 * Deinitializes OneWire interface driver
 *
 * @param[in] ctx OneWire Context
 */
error_t onewire_deinit(onewire_t * ctx);

/**
 * Resets all devices on the line
 *
 * @param[in] ctx OneWire Context
 */
error_t onewire_reset(onewire_t * ctx);

/**
 * Sends one byte to the bus
 *
 * @param[in] ctx OneWire Context
 * @param[in] data Byte to send
 */
error_t onewire_send_byte(onewire_t * ctx, uint8_t data);

/**
 * Sends payload to the bus
 *
 * @param[in] ctx OneWire Context
 * @param[in] payload Data array to send
 * @param[in] size Size of payload
 */
error_t onewire_send_bytes(onewire_t * ctx, uint8_t * payload, size_t size);

/**
 * Receive one byte from the bus
 *
 * @param[in]  ctx OneWire Context
 * @param[out] data Received byte
 */
error_t onewire_recv_byte(onewire_t * ctx, uint8_t * data);

/**
 * Detects all devices on the bus
 *
 * @param[in]     ctx OneWire Context
 * @param[out]    devices Pointer to array of devices
 * @param[in/out] size Pointer to devices array size,
 *                     outputs number of found devices
 */
error_t onewire_detect(
    onewire_t * ctx,
    onewire_device_t * devices,
    size_t * size
);

/**
 * Send command to specific device
 *
 * @param[in] ctx OneWire Context
 * @param[in] dev Device
 * @param[in] command Command byte
 */
error_t onewire_send(onewire_t * ctx, onewire_device_t * dev, uint8_t command);

/**
 * Send payload to specific device
 *
 * @param[in] ctx OneWire Context
 * @param[in] dev Device
 * @param[in] payload Data array to send
 * @param[in] size Size of payload
 */
error_t onewire_send_payload(onewire_t * ctx, onewire_device_t * dev,
                             uint8_t * payload, size_t size);

/**
 * Send command to all devices
 *
 * @param[in] ctx OneWire Context
 * @param[in] command Command byte
 */
error_t onewire_broadcast(onewire_t * ctx, uint8_t command);

/**
 * Send payload to all devices
 *
 * @param[in] ctx OneWire Context
 * @param[in] payload Data array to send
 * @param[in] size Size of payload
 */
error_t onewire_broadcast_payload(onewire_t * ctx, uint8_t * payload,
                                  size_t size);

#ifdef __cplusplus
}
#endif