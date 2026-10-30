#pragma once

#include <stdint.h>
#include <error.h>

typedef struct tty tty_t;
typedef struct tty_input tty_input_t;

typedef error_t (*tty_ansi_seq_processor_t)(tty_t * tty, tty_input_t * input, int c);

/**
 * @brief Input processing context (used by tty_read_line*)
 */
typedef struct tty_input {
  /** User-provided buffer for line data to be put into */
  char * buf;

  /** Current size of line */
  size_t size;

  /** Cursor position within the like buffer */
  uint16_t cursor;

  /** Line input state. Used for processing ANSI escape sequences */
  enum {
    /** Normal operation */
    TTY_INPUT_STATE_DEFAULT = 0,

    /** ESC - escape ('\x1b') was encountered */
    TTY_INPUT_STATE_ESC,

    /** CSI - Control Sequence Introducer ('ESC [') was encountered */
    TTY_INPUT_STATE_CSI,

    /** DCS - Device Control String ('ESC P') was encountered */
    TTY_INPUT_STATE_DCS,

    /** OSC - Operating System Command ('ESC ]') was encountered */
    TTY_INPUT_STATE_OSC,
  } state;

  enum {
    /** Custom state used by tty_process_ansi_*_custom */
    TTY_INPUT_CUSTOM_STATE_DEFAULT = 0,

    /** Can be used to define additional states for tty_process_ansi_*_custom */
#ifdef TTY_INPUT_CUSTOM_STATE
    TTY_INPUT_CUSTOM_STATE
#endif
  } custom_state;

  struct {
    /**
     * @brief Handler for custom CSI escape sequences
     * Called for unexpected chars after CSI - Control Sequence Introducer ('ESC [')
     *
     * @note TTY_LINE_INPUT_CUSTOM_STATE define can be used to add custom states to
     *       tty line input state machine
     * @note For the duration of sequence parsing, the line->input.state must be
     *       TTY_LINE_INPUT_STATE_CSI
     * @note Must set line input state to TTY_LINE_INPUT_STATE_DEFAULT when done parsing
     *
     * @param tty   TTY Context
     * @param input Input Processing Context
     * @param c     Received char
     * @return E_AGAIN to wait for next char, E_OK to process current char as normal
     */
    tty_ansi_seq_processor_t csi;

    /**
     * @brief Handler for custom DCS escape sequences
     * Called for unexpected chars after DCS - Device Control String ('ESC P')
     *
     * @note Defined as weak in tty.c
     * @note TTY_LINE_INPUT_CUSTOM_STATE define can be used to add custom states to
     *       tty line input state machine
     * @note For the duration of sequence parsing, the line->input.state must be
     *       TTY_LINE_INPUT_STATE_DCS
     * @note Must set line input state to TTY_LINE_INPUT_STATE_DEFAULT when done parsing
     *
     * @param tty   TTY Context
     * @param input Input Processing Context
     * @param c     Received char
     * @return E_AGAIN to wait for next char, E_OK to process current char as normal
     */
    tty_ansi_seq_processor_t dcs;

    /**
     * @brief Handler for custom OSC escape sequences
     * Called for unexpected chars after OSC - Operating System Command ('ESC ]')
     *
     * @note Defined as weak in tty.c
     * @note TTY_LINE_INPUT_CUSTOM_STATE define can be used to add custom states to
     *       tty line input state machine
     * @note For the duration of sequence parsing, the line->input.state must be
     *       TTY_LINE_INPUT_STATE_OSC
     * @note Must set line input state to TTY_LINE_INPUT_STATE_DEFAULT when done parsing
     *
     * @param tty   TTY Context
     * @param input Input Processing Context
     * @param c     Received char
     * @return E_AGAIN to wait for next char, E_OK to process current char as normal
     */
    tty_ansi_seq_processor_t osc;
  } custom;

  struct {
    void * csi;
    void * dcs;
    void * osc;
  } ctx;
} tty_input_t;

/**
 * @brief Resets input state (clears size/cursor & sets buffer to NULL)
 */
error_t tty_input_reset(tty_input_t * input);

/**
 * @brief Clears input state (clears size/cursor)
 */
error_t tty_input_clear(tty_input_t * input);
