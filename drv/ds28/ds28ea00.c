/** ========================================================================= *
 *
 * @file ds28ea00.c
 * @date 23-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "ds28/ds28ea00.h"
#include "error/assertion.h"
#include "util/bits.h"
#include "log/log.h"
#include <string.h>

/* Defines ================================================================== */
#define LOG_TAG DS28

/** DS28EA00 Family code, used to detect DS28EA00 devices on the line */
#define DS28EA00_FAMILY_CODE 0x42

/** DS28EA00 Specific commands */
#define DS28EA00_CMD_CONVERT_TEMP     0x44
#define DS28EA00_CMD_WRITE_SCRATCHPAD 0x4E
#define DS28EA00_CMD_READ_SCRATCHPAD  0xBE
#define DS28EA00_CMD_COPY_SCRATCHPAD  0x48
#define DS28EA00_CMD_READ_PWR_MODE    0xB4
#define DS28EA00_CMD_COND_READ_ROM    0x0F
#define DS28EA00_CMD_PIO_WRITE        0xA5
#define DS28EA00_CMD_PIO_READ         0xF5
#define DS28EA00_CMD_CHAIN            0x99

/** Chain commands */
#define DS28EA00_CHAIN_ON   0x5A
#define DS28EA00_CHAIN_OFF  0x3C
#define DS28EA00_CHAIN_DONE 0x96

/** DS28EA00 returns this value as OK response to a chain command */
#define DS28EA00_VALID 0xAA

/** Value that is returned when no more devices are present during chain
 * detection */
#define DS28EA00_CHAIN_DETECT_END 0xFF

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
static error_t ds28ea00_write_scratchpad(
    ds28ea00_t * ctx, ds28ea00_device_t * device) {
  ASSERT_RETURN(ctx && device, E_NULL);

  uint8_t payload[] = {
    DS28EA00_CMD_WRITE_SCRATCHPAD,
    device->scratchpad.temp_alarm.hi,
    device->scratchpad.temp_alarm.lo,
    device->scratchpad.config_register,
  };

  onewire_reset(ctx->ow);
  onewire_send_payload(ctx->ow, &device->ow_dev, payload, UTIL_ARR_SIZE(payload));
  onewire_reset(ctx->ow);

  return E_OK;
}

static error_t ds28ea00_copy_scratchpad(
    ds28ea00_t * ctx, ds28ea00_device_t * device) {
  ASSERT_RETURN(ctx && device, E_NULL);

  onewire_reset(ctx->ow);
  onewire_send(ctx->ow, &device->ow_dev, DS28EA00_CMD_COPY_SCRATCHPAD);

  uint8_t data;
  do {
    onewire_recv_byte(ctx->ow, &data);
  } while (data != 0xFF);

  return E_OK;
}

/* Shared functions ========================================================= */
error_t ds28ea00_init(
    ds28ea00_t * ctx, onewire_t * ow, ds28ae00_init_mode_t init_mode) {
  ASSERT_RETURN(ctx && ow, E_NULL);

  ctx->ow = ow;

  memset(ctx->devices.items, 0, sizeof(ctx->devices.items));

  ctx->devices.size = UTIL_ARR_SIZE(ctx->devices.items);

  switch (init_mode) {
    case DS28EA00_INIT_MODE_DEFAULT:
      ERROR_CHECK_RETURN(
          ds28ea00_detect(ctx, ctx->devices.items, &ctx->devices.size));
      break;
    case DS28EA00_INIT_MODE_CHAIN:
      ERROR_CHECK_RETURN(
          ds28ea00_sequence_detect(ctx, ctx->devices.items, &ctx->devices.size));
      break;
    default:
      return E_INVAL;
  }

  for (size_t i = 0; i < ctx->devices.size; ++i) {
    onewire_reset(ctx->ow);
    ds28ea00_read_power_mode(
        ctx,
        &ctx->devices.items[i],
        &ctx->devices.items[i].pwr_mode
    );
  }

  log_info("Found %d DS28EA00 devices", ctx->devices.size);
  for (size_t i = 0; i < ctx->devices.size; ++i) {
#define FH2 "%02x "
#define ROM(idx) ctx->devices.items[i].ow_dev.rom.buffer[idx]
    log_info(
        "Device #%d [%s]: "
        FH2 FH2 FH2 FH2 FH2 FH2 FH2 FH2,
        ctx->devices.items[i].chain_position,
        ctx->devices.items[i].pwr_mode == DS28EA00_PWR_MODE_VDD ? "VDD" : " 1W",
        ROM(0), ROM(1), ROM(2), ROM(3),
        ROM(4), ROM(5), ROM(6), ROM(7)
    );
#undef FH2
#undef ROM
  }

  return E_OK;
}

error_t ds28ea00_deinit(ds28ea00_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  memset(ctx->devices.items, 0, sizeof(ctx->devices.items));

  return E_OK;
}

error_t ds28ea00_detect(
    ds28ea00_t * ctx, ds28ea00_device_t * devices, size_t * size) {
  ASSERT_RETURN(ctx && devices && size, E_NULL);

  onewire_device_t all_devices[DS28EA00_MAX_DEVICES] = {0};

  size_t found = UTIL_ARR_SIZE(all_devices);
  ERROR_CHECK_RETURN(onewire_detect(ctx->ow, all_devices, &found));

  size_t devices_idx = 0;
  for (size_t i = 0; i < found; ++i) {
    if (all_devices[i].rom.family_code == DS28EA00_FAMILY_CODE) {
      memcpy(&devices[devices_idx].ow_dev, &all_devices[i], sizeof(onewire_device_t));

      if (++devices_idx > *size) {
        *size = devices_idx;
        return E_OVERFLOW;
      }
    }
  }

  *size = devices_idx;

  return E_OK;
}

error_t ds28ea00_sequence_detect(
    ds28ea00_t * ctx, ds28ea00_device_t * devices, size_t * size) {
  ASSERT_RETURN(ctx && devices, E_NULL);

  uint8_t on_payload[] = {
      DS28EA00_CMD_CHAIN, DS28EA00_CHAIN_ON, ~DS28EA00_CHAIN_ON
  };

  onewire_reset(ctx->ow);
  onewire_broadcast_payload(ctx->ow, on_payload, UTIL_ARR_SIZE(on_payload));

  uint8_t data = 0;
  onewire_recv_byte(ctx->ow, &data);

  if (data != DS28EA00_VALID) {
    *size = 0;
    return E_INVAL;
  }

  uint8_t end = 0, idx = 0;

  do {
    onewire_reset(ctx->ow);
    onewire_send_byte(ctx->ow, DS28EA00_CMD_COND_READ_ROM);

    end = DS28EA00_CHAIN_DETECT_END;

    for (uint8_t i = 0; i < OW_ROM_SIZE; ++i) {
      onewire_recv_byte(ctx->ow, &data);
      end &= data;
      devices[idx].ow_dev.rom.buffer[i] = data;
    }

    if (end == DS28EA00_CHAIN_DETECT_END) {
      break;
    }

    devices[idx].chain_position = idx;
    idx++;

    uint8_t done_payload[] = {
        DS28EA00_CMD_PIO_WRITE,
        DS28EA00_CMD_CHAIN, DS28EA00_CHAIN_DONE, ~DS28EA00_CHAIN_DONE
    };

    onewire_reset(ctx->ow);
    onewire_send_bytes(ctx->ow, done_payload, UTIL_ARR_SIZE(done_payload));

    onewire_recv_byte(ctx->ow, &data);

    /* FIXME: On STM32L051 and DS28EA00
     *        chain detection algorithm reads first byte of every response
     *        with lsb bit flipped, although logic analyzer reads correct
     *        values. Fast fix is to detect flipped bits and flip them back
     *        which is done here.
     */
#if USE_DS28EA00_CHAIN_DETECT_BIT_FLIP_FIX
    if (data == UTIL_BIT_TOGGLE(DS28EA00_VALID, 0)) {
      data = UTIL_BIT_TOGGLE(data, 0);
      uint8_t * first_byte = &devices[idx-1].ow_dev.rom.buffer[0];
      *first_byte = UTIL_BIT_TOGGLE(*first_byte, 0);
    }
#endif

    if (data != DS28EA00_VALID) {
      *size = idx;
      return E_INVAL;
    }
  } while (end != DS28EA00_CHAIN_DETECT_END);


  uint8_t off_payload[] = {
      DS28EA00_CMD_CHAIN, DS28EA00_CHAIN_OFF, ~DS28EA00_CHAIN_OFF
  };

  onewire_reset(ctx->ow);
  onewire_broadcast_payload(ctx->ow, off_payload, UTIL_ARR_SIZE(off_payload));

  onewire_recv_byte(ctx->ow, &data);

  if (data != DS28EA00_VALID) {
    *size = idx;
    return E_INVAL;
  }

  *size = idx;

  return E_OK;
}

error_t ds28ea00_convert_temp(ds28ea00_t * ctx, ds28ea00_device_t * target) {
  ASSERT_RETURN(ctx, E_NULL);

  onewire_reset(ctx->ow);

  return target
      ? onewire_send(ctx->ow, &target->ow_dev, DS28EA00_CMD_CONVERT_TEMP)
      : onewire_broadcast(ctx->ow, DS28EA00_CMD_CONVERT_TEMP);
}

error_t ds28ea00_read_temp(
    ds28ea00_t * ctx, ds28ea00_device_t * target, ds28ea00_temp_t * temp) {
  ASSERT_RETURN(ctx && temp, E_NULL);

  onewire_reset(ctx->ow);

  ERROR_CHECK_RETURN(
      target
        ? onewire_send(ctx->ow, &target->ow_dev, DS28EA00_CMD_READ_SCRATCHPAD)
        : onewire_broadcast(ctx->ow, DS28EA00_CMD_READ_SCRATCHPAD));

  uint8_t temp_lsb, temp_msb;

  onewire_recv_byte(ctx->ow, &temp_lsb);
  onewire_recv_byte(ctx->ow, &temp_msb);

  uint16_t temp_val = ((uint16_t) temp_msb << 8) | temp_lsb;

  // Use scaling to avoid using FP arithmetic
  temp->value = temp_val / DS28EA00_SCALER;
  temp->fraction = (temp_val * DS28EA00_PRECISION / DS28EA00_SCALER) % DS28EA00_PRECISION;

  return E_OK;
}

error_t ds28ea00_read_power_mode(
    ds28ea00_t * ctx, ds28ea00_device_t * target, ds28ea00_pwr_mode_t * pwr) {
  ASSERT_RETURN(ctx && pwr, E_NULL);

  onewire_send(ctx->ow, &target->ow_dev, DS28EA00_CMD_READ_PWR_MODE);
  onewire_recv_byte(ctx->ow, pwr);

  return E_OK;
}

error_t ds28ea00_set_alarm(
    ds28ea00_t * ctx, ds28ea00_device_t * target, uint16_t alarm_temp) {
  ASSERT_RETURN(ctx && target, E_NULL);

  target->scratchpad.temp_alarm.value = alarm_temp;
  ds28ea00_write_scratchpad(ctx, target);
  ds28ea00_copy_scratchpad(ctx, target);

  return E_OK;
}

error_t ds28ea00_set_alarm_lo(
    ds28ea00_t * ctx, ds28ea00_device_t * target, uint8_t alarm_temp_lo) {
  ASSERT_RETURN(ctx && target, E_NULL);

  target->scratchpad.temp_alarm.lo = alarm_temp_lo;
  ds28ea00_write_scratchpad(ctx, target);
  ds28ea00_copy_scratchpad(ctx, target);

  return E_OK;
}

error_t ds28ea00_set_alarm_hi(
    ds28ea00_t * ctx, ds28ea00_device_t * target, uint8_t alarm_temp_hi) {
  ASSERT_RETURN(ctx && target, E_NULL);

  target->scratchpad.temp_alarm.hi = alarm_temp_hi;
  ds28ea00_write_scratchpad(ctx, target);
  ds28ea00_copy_scratchpad(ctx, target);

  return E_OK;
}

error_t ds28ea00_set_resolution(
    ds28ea00_t * ctx,
    ds28ea00_device_t * target,
    ds28ea00_resolution_t resolution
) {
  ASSERT_RETURN(ctx && target, E_NULL);

  target->scratchpad.config_register = resolution;
  ds28ea00_write_scratchpad(ctx, target);
  ds28ea00_copy_scratchpad(ctx, target);

  return E_OK;
}
