/** ========================================================================= *
 *
 * @file onewire.c
 * @date 24-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "hal/onewire/onewire.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG                   HAL_1W

#define OW_BAUDRATE_STARTUP       9600
#define OW_BAUDRATE_OPERATIONAL   115200
#define OW_RESET                  0xF0
#define OW_0                      0x00
#define OW_1                      0xFF

#define OW_START_SEARCH 0xFF
#define OW_LAST_DEVICE  0x00

/* Macros =================================================================== */
#define OW_VAL_TO_BIT(val) ((val) ? OW_1 : OW_0)
#define OW_BIT_TO_VAL(bit) ((bit) == OW_1 ? 1 : 0)

/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
static uint8_t onewire_send_recv_bit(onewire_t * ctx, uint8_t bit) {
  ctx->trsp.send(ctx->trsp.ctx, OW_VAL_TO_BIT(bit));
  return OW_BIT_TO_VAL(ctx->trsp.recv(ctx->trsp.ctx));
}

static uint32_t onewire_detect_one(
    onewire_t * ctx,
    uint8_t next_node,
    onewire_device_t * dev
) {
  uint32_t last_forked_point = 0;

  // TODO: signal an error
  onewire_reset(ctx);

  onewire_send_byte(ctx, OW_CMD_SEARCH_ROM);

  for (uint8_t i = 64 ; i > 0 ; i--) {
    uint8_t byte = OW_ROM_SIZE - ((i - 1) / 8) - 1;

    uint8_t bit = onewire_send_recv_bit(ctx, 1);

    if (onewire_send_recv_bit(ctx, 1)) {
      if (bit) {
        last_forked_point = 0;
        break;
      }
    } else if (!bit) {
      if (next_node > i || (next_node != i && (dev->rom.buffer[byte] & 1))) {
        bit = 1;
        last_forked_point = 1;
      }
    }

    onewire_send_recv_bit(ctx, bit);

    dev->rom.buffer[byte] >>= 1;

    if (bit) {
      dev->rom.buffer[byte] |= 0x80;
    }
  }

  return last_forked_point;
}

/* Shared functions ========================================================= */
error_t onewire_init(onewire_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);
  return E_OK;
}

error_t onewire_deinit(onewire_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);
  return E_OK;
}

error_t onewire_reset(onewire_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  ctx->trsp.set_baudrate(ctx->trsp.ctx, OW_BAUDRATE_STARTUP);
  ctx->trsp.send(ctx->trsp.ctx, OW_RESET);
  uint8_t presence = ctx->trsp.recv(ctx->trsp.ctx);
  ctx->trsp.set_baudrate(ctx->trsp.ctx, OW_BAUDRATE_OPERATIONAL);

  log_info("reset: 0x%02x", presence);

  return presence == OW_RESET ? E_NORESP : E_OK;
}

error_t onewire_send_byte(onewire_t * ctx, uint8_t data) {
  ASSERT_RETURN(ctx, E_NULL);

  for (uint8_t i = 0; i < 8; ++i) {
    ctx->trsp.send(ctx->trsp.ctx, OW_VAL_TO_BIT(data & (1 << i)));
  }

  return E_OK;
}

error_t onewire_send_bytes(onewire_t * ctx, uint8_t * payload, size_t size) {
  ASSERT_RETURN(ctx && payload, E_NULL);

  for (size_t i = 0; i < size ; ++i) {
    onewire_send_byte(ctx, payload[i]);
  }

  return E_OK;
}

error_t onewire_recv_byte(onewire_t * ctx, uint8_t * data) {
  ASSERT_RETURN(ctx && data, E_NULL);

  *data = 0;

  for (uint8_t i = 0; i < 8; ++i) {
    ctx->trsp.send(ctx->trsp.ctx, OW_1);
    if (ctx->trsp.recv(ctx->trsp.ctx) == OW_1) {
      *data |= (1 << i);
    }
  }

  log_debug("recv: 0x%02x", *data);

  return E_OK;
}

error_t onewire_detect(
    onewire_t * ctx,
    onewire_device_t * devices,
    size_t * size
) {
  ASSERT_RETURN(ctx && devices && size, E_NULL);
  ASSERT_RETURN(*size > 0, E_INVAL);

  size_t device_index = 0;
  uint8_t next_device = OW_START_SEARCH;

  while (next_device != OW_LAST_DEVICE) {
    next_device = onewire_detect_one(ctx, next_device, &devices[device_index]);

#define FH2 "%02x "
#define ROM(idx) devices[device_index].rom.buffer[idx]
    log_info(
        "onewire_search: "
        FH2 FH2 FH2 FH2 FH2 FH2 FH2 FH2,
        ROM(0), ROM(1), ROM(2), ROM(3),
        ROM(4), ROM(5), ROM(6), ROM(7)
    );
#undef FH2
#undef ROM

    if (++device_index > *size) {
      *size = device_index;
      return E_OVERFLOW;
    }
  }

  *size = device_index;

  return E_OK;
}

error_t onewire_send(onewire_t * ctx, onewire_device_t * dev, uint8_t command) {
  ASSERT_RETURN(ctx && dev, E_NULL);

  onewire_send_byte(ctx, OW_CMD_MATCH_ROM);

  for (uint8_t i = 0; i < OW_ROM_SIZE; ++i) {
    onewire_send_byte(ctx, dev->rom.buffer[i]);
  }

  onewire_send_byte(ctx, command);

  return E_OK;
}

error_t onewire_send_payload(
    onewire_t * ctx, onewire_device_t * dev, uint8_t * payload, size_t size) {
  ASSERT_RETURN(ctx && dev && payload, E_NULL);

  onewire_send_byte(ctx, OW_CMD_MATCH_ROM);

  for (uint8_t i = 0; i < OW_ROM_SIZE; ++i) {
    onewire_send_byte(ctx, dev->rom.buffer[i]);
  }

  return onewire_send_bytes(ctx, payload, size);
}

error_t onewire_broadcast(onewire_t * ctx, uint8_t command) {
  ASSERT_RETURN(ctx, E_NULL);

  onewire_send_byte(ctx, OW_CMD_SKIP_ROM);
  onewire_send_byte(ctx, command);

  return E_OK;
}

error_t onewire_broadcast_payload(
    onewire_t * ctx, uint8_t * payload, size_t size) {
  ASSERT_RETURN(ctx && payload, E_NULL);

  onewire_send_byte(ctx, OW_CMD_SKIP_ROM);

  return onewire_send_bytes(ctx, payload, size);
}
