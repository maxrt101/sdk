/** ========================================================================= *
 *
 * @file line.h
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief TTY Line
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

/* Macros =================================================================== */
/* Enums ==================================================================== */
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

/** Forward declaration of tty_t for tty_process_ansi_*_custom */
typedef struct tty_t tty_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
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
 * Return `const char *` from tty_line_t
 *
 * @note Ensures that line is properly terminated
 *
 * @param line TTY Line Context
 */
const char * tty_line_get_str(tty_line_t * line);

/**
 * Initialize TTY Line from buffer
 *
 * @param line TTY Line Context
 * @param buf Buffer to copy into line
 * @param size Buffer size
 */
error_t tty_line_from_buf(tty_line_t * line, const char * buf, size_t size);

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