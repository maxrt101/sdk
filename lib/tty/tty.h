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
 * @return TTY Line
 */
typedef struct {
  /** Line buffer */
  char buf[TTY_MAX_LINE_SIZE];

  /** Line size */
  uint16_t size;

  /** Input processing context (used by tty_read_line*) */
  struct {
    /** Cursor position within the like buffer */
    uint16_t cursor;

    /** Line input state. Used for processing ANSI escape sequences */
    __PACKED_ENUM {
      /** Normal operation */
      TTY_LINE_INPUT_STATE_DEFAULT = 0,

      /** ESC - escape ('\x1b') was encountered */
      TTY_LINE_INPUT_STATE_ESC,

      /** CSI - Control Sequence Introducer ('ESC [') was encountered */
      TTY_LINE_INPUT_STATE_CSI,

      /** DCS - Device Control String ('ESC P') was encountered */
      TTY_LINE_INPUT_STATE_DCS,

      /** OSC - Operating System Command ('ESC ]') was encountered */
      TTY_LINE_INPUT_STATE_OSC,
    } state;

    __PACKED_ENUM {
      /** Custom state used by tty_process_ansi_*_custom */
      TTY_LINE_INPUT_CUSTOM_STATE_DEFAULT = 0,

      /** Can be used to define additional states for tty_process_ansi_*_custom */
#ifdef TTY_LINE_INPUT_CUSTOM_STATE
      TTY_LINE_INPUT_CUSTOM_STATE
#endif
    } custom_state;
  } input;
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
 * Reset TTY Line
 *
 * @param line TTY Line
 */
error_t tty_line_reset(tty_line_t * line);

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
 * Handler for custom CSI escape sequences
 * Called for unexpected chars after CSI - Control Sequence Introducer ('ESC [')
 *
 * @note Defined as weak in tty.c
 * @note TTY_LINE_INPUT_CUSTOM_STATE define can be used to add custom states to
 *       tty line input state machine
 * @note For the duration of sequence parsing, the line->input.state must be
 *       TTY_LINE_INPUT_STATE_CSI
 * @note Must set line input state to TTY_LINE_INPUT_STATE_DEFAULT when done parsing
 *
 * @param tty  TTY Context
 * @param line Line buffer
 * @param c    Received char
 * @return E_AGAIN to wait for next char, E_OK to process current char as normal
 */
error_t tty_process_ansi_csi_custom(tty_t * tty, tty_line_t * line, char c);

/**
 * Handler for custom DCS escape sequences
 * Called for unexpected chars after DCS - Device Control String ('ESC P')
 *
 * @note Defined as weak in tty.c
 * @note TTY_LINE_INPUT_CUSTOM_STATE define can be used to add custom states to
 *       tty line input state machine
 * @note For the duration of sequence parsing, the line->input.state must be
 *       TTY_LINE_INPUT_STATE_DCS
 * @note Must set line input state to TTY_LINE_INPUT_STATE_DEFAULT when done parsing
 *
 * @param tty  TTY Context
 * @param line Line buffer
 * @param c    Received char
 * @return E_AGAIN to wait for next char, E_OK to process current char as normal
 */
error_t tty_process_ansi_dcs_custom(tty_t * tty, tty_line_t * line, char c);

/**
 * Handler for custom OSC escape sequences
 * Called for unexpected chars after OSC - Operating System Command ('ESC ]')
 *
 * @note Defined as weak in tty.c
 * @note TTY_LINE_INPUT_CUSTOM_STATE define can be used to add custom states to
 *       tty line input state machine
 * @note For the duration of sequence parsing, the line->input.state must be
 *       TTY_LINE_INPUT_STATE_OSC
 * @note Must set line input state to TTY_LINE_INPUT_STATE_DEFAULT when done parsing
 *
 * @param tty  TTY Context
 * @param line Line buffer
 * @param c    Received char
 * @return E_AGAIN to wait for next char, E_OK to process current char as normal
 */
error_t tty_process_ansi_osc_custom(tty_t * tty, tty_line_t * line, char c);

#ifdef __cplusplus
}
#endif