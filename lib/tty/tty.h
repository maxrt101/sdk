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
#include "tty/line.h"
#include <stdbool.h>

/* Defines ================================================================== */
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
 * ASCII value for BEL
 */
#ifndef TTY_ASCII_KEY_BEL
#define TTY_ASCII_KEY_BEL '\a'
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
  TTY_FLAG_USE_BELL   = (1 << 1),
} tty_flag_t;

/* Types ==================================================================== */
/**
 * TTY Context
 */
typedef struct tty_t {
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
 * Read 1 char from TTY input (blocking)
 *
 * @param tty TTY Context
 * @param ch Where to put read char
 */
error_t tty_get_char(tty_t * tty, char * ch);

/**
 * Read 1 char from TTY input (async)
 *
 * @param tty TTY Context
 * @param ch Where to put read char
 */
error_t tty_get_char_async(tty_t * tty, char * ch);

/**
 * Read line from TTY input (blocking) (@ref tty_flag_t)
 *
 * @param tty TTY Context
 * @param line Line buffer to raed into
 */
error_t tty_read_line(tty_t * tty, tty_line_t * line);

/**
 * Read line from TTY input (async) (@ref tty_flag_t)
 *
 * @note If no char is detected, will return with E_AGAIN
 * @note When CR LF is received - will terminate line & return E_OK
 *
 * @param tty TTY Context
 * @param line Line buffer to raed into
 */
error_t tty_read_line_async(tty_t * tty, tty_line_t * line);

/**
 * Write line to TTY output (@ref tty_flag_t)
 *
 * @param tty TTY Context
 * @param line Line buffer to write from
 */
error_t tty_write_line(tty_t * tty, tty_line_t * line);

/**
 * Write formatted line to TTY output
 *
 * @param tty TTY Context
 * @param fmt Format string
 * @param ... Format arguments
 */
error_t tty_write_fmt(tty_t * tty, const char * fmt, ...);

/**
 * Signal with ASCII BEL char, if appropriate flag is set
 *
 * @param tty TTY Context
 */
error_t tty_bell(tty_t * tty);


#ifdef __cplusplus
}
#endif