/**
 * @brief Generic API for IO Objects - Devices
 *
 * A device consists of a context & device API.
 * Device API consists of open, close, read, write & ioctl methods.
 *
 * Device APIs live in an LDS
 *
 * Example can be found in host platform cio.c
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <error.h>
#include <lds.h>

/**
 * @brief Declare an API named `__name`
 *
 * Example:
 * @code{.c}
    DEVICE_API_DECLARE(uart, {
      .open  = uart_open,
      .close = uart_close,
      .read  = uart_read,
      .write = uart_write,
      .ioctl = uart_ioctl,
    });
 * @endcode
 */
#define DEVICE_API_DECLARE(__name, ...) \
  LDS_APPEND_NAMED(device_api, __name, __VA_ARGS__)

/**
 * @brief Place in a header to make declared API available everywhere
 *
 * Example:
 * @code{.c}
    DEVICE_API_EXPORT(uart);
 * @endcode
 */
#define DEVICE_API_EXPORT(__name) \
  LDS_EXPORT_ITEM(device_api, __name)

/**
 * @brief Declare a device using provided API and context
 *
 * Example:
 * @code{.c}
    struct uart_ctx {
      void * handle;
    } uart1_ctx = {UART1};

    DEVICE_DECLARE(console, uart, &uart1_ctx);
 * @endcode
 */
#define DEVICE_DECLARE(__name, __api, __ctx) \
  device_t __name = { .api = &__api, .ctx = __ctx }

/**
 * @brief Place in a header to make declared device available everywhere
 * Example:
 * @code{.c}
    DEVICE_EXPORT(console);
 * @endcode
 */
#define DEVICE_EXPORT(__name) \
  extern device_t __name

/**
 * @brief Device IOCTL commands
 */
typedef enum dev_ioctl {
  DEV_IOCTL_RESERVED_0        = 0,
  DEV_IOCTL_IS_CMD_SUPPORTED  = 1,
  DEV_IOCTL_SET_BLOCKING      = 2,
  DEV_IOCTL_SEEK              = 3,
  DEV_IOCTL_TELL              = 4,
  DEV_IOCTL_GET_PAGE_SIZE     = 5,
} dev_ioctl_t;

typedef struct device device_t;

/**
 * @brief Device API table
 */
typedef struct device_api {
  error_t (*open)(device_t * d, va_list ap);
  error_t (*close)(device_t * d);
  error_t (*read)(device_t * d, uint8_t * buf, size_t len);
  error_t (*write)(device_t * d, const uint8_t * buf, size_t len);
  error_t (*ioctl)(device_t * d, int cmd, va_list ap);
} device_api_t;

/**
 * @brief Device (API + context)
 */
typedef struct device {
  const device_api_t * api;
  void * ctx;
} device_t;

/**
 * @brief API tables are static - so they live in an LDS
 */
LDS_DECLARE(device_api, device_api_t);

error_t device_open(device_t * d, ...);
error_t device_close(device_t * d);
error_t device_read(device_t * d, uint8_t * buf, size_t len);
error_t device_write(device_t * d, const uint8_t * buf, size_t len);
error_t device_ioctl(device_t * d, int cmd, ...);

bool device_supports_ioctl(device_t * d, dev_ioctl_t cmd);

// error_t device_read_from(device_t * d, size_t ofs, uint8_t * buf, size_t len);
// error_t device_write_to(device_t * d, size_t ofs, const uint8_t * buf, size_t len);


#define DEVICE_SUPER_EXPERIMENTAL_AUTO_GENERATE_IOCTL_FN 0
#if DEVICE_SUPER_EXPERIMENTAL_AUTO_GENERATE_IOCTL_FN
#define DEVICE_IOCTL_ARG_SIG_IMPL(__t, __n) , __t __n
#define DEVICE_IOCTL_ARG_SIG_TRAMPOLINE(...) DEVICE_IOCTL_ARG_SIG_IMPL(__VA_ARGS__)
#define DEVICE_IOCTL_ARG_SIG(...) DEVICE_IOCTL_ARG_SIG_TRAMPOLINE(UTIL_EXPAND __VA_ARGS__)

#define DEVICE_IOCTL_ARG_NAME_IMPL(__t, __n) , __n
#define DEVICE_IOCTL_ARG_NAME_TRAMPOLINE(...) DEVICE_IOCTL_ARG_NAME_IMPL(__VA_ARGS__)
#define DEVICE_IOCTL_ARG_NAME(...) DEVICE_IOCTL_ARG_NAME_TRAMPOLINE(UTIL_EXPAND __VA_ARGS__)

#define DEVICE_IOCTL_GENERATE_FN(__id, __name, ...) \
  static inline error_t UTIL_CAT(device_, __name)(device_t * d UTIL_FOR_EACH(DEVICE_IOCTL_ARG_SIG, __VA_ARGS__)) {  \
    ASSERT_RETURN(device_supports_ioctl(d, __id), E_NOTIMPL);                                                       \
    return device_ioctl(d, __id UTIL_FOR_EACH(DEVICE_IOCTL_ARG_NAME, __VA_ARGS__));                                 \
  }

#define X DEVICE_IOCTL_GENERATE_FN

X(DEV_IOCTL_SEEK, seek, (size_t, size))
X(DEV_IOCTL_TELL, tell, (size_t *, size))
#endif
