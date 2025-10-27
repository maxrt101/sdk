/** ========================================================================= *
 *
 * @file tty.h
 * @date 09-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief TTY Emulation
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/compiler.h"
#include "vfs/vfs.h"
#include <stdbool.h>

/* Defines ================================================================== */
/**
 * Max size of line
 */
#ifndef TTY_MAX_LINE_SIZE
#define TTY_MAX_LINE_SIZE 64
#endif

/**
 * ASCII value for backspace
 */
#ifndef TTY_ASCII_KEY_BACKSPACE
#define TTY_ASCII_KEY_BACKSPACE 0x08
#endif

/**
 * ASCII value for ESC
 */
#ifndef TTY_ASCII_KEY_ESC
#define TTY_ASCII_KEY_ESC 0x1B
#endif

/**
 * UART sends CR if enter is pressed
 */
#ifndef TTY_ASCII_KEY_ENTER
#define TTY_ASCII_KEY_ENTER '\r'
#endif

/* Macros =================================================================== */
/* Enums ==================================================================== */
/**
 * TTY Flags
 */
typedef enum {
  TTY_FLAG_ECHO_INPUT = (1 << 0),
} tty_flag_t;

/* Types ==================================================================== */
/**
 * @return TTY Line (char buffer with size)
 */
typedef struct {
  char buf[TTY_MAX_LINE_SIZE];
  uint16_t size;
} tty_line_t;

/**
 * TTY Context
 */
typedef struct {
  /** File handle for IO */
  vfs_file_t * file;

  /** TTY Flags */
  uint8_t flags;
} tty_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initialize TTY Context
 *
 * @param tty TTY Context
 * @param file File for IO
 */
error_t tty_init(tty_t * tty, vfs_file_t * file);

/**
 * Reset TTY
 *
 * @param tty TTY Context
 */
error_t tty_reset(tty_t * tty);

/**
 * Initialize TTY Line from c string
 *
 * @param line TTY Line Context
 * @param str String to copy into line
 */
error_t tty_line_from_str(tty_line_t * line, const char * str);

/**
 * Initialize TTY Line from buffer
 *
 * @param line TTY Line Context
 * @param buf Buffer to copy into line
 * @param size Buffer size
 */
error_t tty_line_from_buf(tty_line_t * line, const char * buf, size_t size);

/**
 * Set TTY Flag (@ref tty_flag_t)
 *
 * @param tty TTY Context
 * @param flag Flag to set
 */
error_t tty_set_flag(tty_t * tty, tty_flag_t flag);

/**
 * Clear TTY Flag (@ref tty_flag_t)
 *
 * @param tty TTY Context
 * @param flag Flag to clear
 */
error_t tty_clear_flag(tty_t * tty, tty_flag_t flag);

/**
 * Gets TTY Flag values (@ref tty_flag_t)
 *
 * @param tty TTY Context
 * @param flag Flag to get value of
 */
bool tty_get_flag(tty_t * tty, tty_flag_t flag);

/**
 * Read line (blocking) (@ref tty_flag_t)
 *
 * @param tty TTY Context
 * @param line Line buffer to raed into
 */
error_t tty_read_line(tty_t * tty, tty_line_t * line);

/**
 * Read line (async) (@ref tty_flag_t)
 *
 * @note If no char is detected, will return with E_AGAIN
 *
 * @param tty TTY Context
 * @param line Line buffer to raed into
 */
error_t tty_read_line_async(tty_t * tty, tty_line_t * line);

/**
 * Write line (@ref tty_flag_t)
 *
 * @param tty TTY Context
 * @param line Line buffer to write from
 */
error_t tty_write_line(tty_t * tty, tty_line_t * line);

#ifdef __cplusplus
}
#endif