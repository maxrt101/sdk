#pragma once

#include <error.h>
#include <device.h>
#include <tty/input.h>

#include <stddef.h>

/**
 * @brief TTY Flags
 */
typedef enum tty_flags {
  TTY_FLAG_NONE = 0,

  /// Echo input back to the device
  TTY_FLAG_ECHO = (1 << 0),

  /// Allow for a bell to be played
  TTY_FLAG_BELL = (1 << 1),

  /// TBD: require for user to log in
  // TTY_FLAG_LOGIN = (1 << 2),

  /// Special flag, that is set if a write operation is performed on a TTY
  /// Can be used to detect writes. Will not be reset by itself
  TTY_FLAG_WRITE_DETECTION = (1 << 7)
} tty_flags_t;

/**
 * @brief TTY Context
 */
typedef struct tty {
  device_t *  dev;   /// Underlying device
  tty_input_t input; /// Input context
  uint8_t     flags; /// Flags
} tty_t;

/**
 * @brief Initialize the TTY, bind `dev` to it
 */
error_t tty_init(tty_t * tty, device_t * dev);

/**
 * @brief Read `len` chars into `buf` from the TTY
 */
error_t tty_read(tty_t * tty, char * buf, size_t len);

/**
 * @brief Read entire line (up to TTY_KEY_ENTER) into `buf`
 * @note Will NULL-terminate
 * @note If in non-blocking mode will return E_AGAIN until ready (E_OK)
 */
error_t tty_readline(tty_t * tty, char * buf, size_t len);

/**
 * @brief Write a buffer `buf` of size `len` to the TTY
 */
error_t tty_write(tty_t * tty, const char * buf, size_t len);

/**
 * @brief Write a null-terminated string to the TTY
 */
error_t tty_puts(tty_t * tty, const char * str);

/**
 * @brief vsprintf for a TTY
 */
error_t tty_vprintf(tty_t * tty, const char * fmt, va_list ap);

/**
 * @brief printf for a TTY
 */
error_t tty_printf(tty_t * tty, const char * fmt, ...) __PRINTF_LIKE(2, 3);

/**
 * @brief Try to set underlying device's blocking mode
 * @note Will return E_OK if successful, other error codes (device-dependant) otherwise
 */
error_t tty_set_blocking(tty_t * tty, bool blocking);

/**
 * @brief Play a bell if enabled
 */
error_t tty_bell(tty_t * tty);

#if WITH_TTY_COLORTEST
/**
 * @brief Perform an ANSI color test
 */
void tty_colortest(tty_t * tty);
#endif
