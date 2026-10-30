#pragma once

#include <tty.h>
#include <sys/types.h>

#if WITH_SHELL_HISTORY

/**
 * @brief Shell history context
 */
typedef struct shell_history {
  /** History buffer */
  struct {
    char line[SHELL_MAX_LINE_SIZE];
  } buffer[SHELL_HISTORY_BUFFER_SIZE];

  /** Index into history. Used by UP/DOWN arrow key handlers */
  ssize_t index;

  /** Points to last command */
  ssize_t head;

  /** Cache previously set CSI handler & it's context to allow for multiple handlers
   * for the same sequence typ */
  tty_ansi_seq_processor_t prev_csi_handler;
  void *                   prev_csi_context;
} shell_history_t;

typedef struct shell shell_t;
typedef struct shell_input shell_input_t;

/**
 * @brief Initializes shell history context
 *
 * Will place special CSI handler into TTY and save previous
 * handler to setup a chain of handlers, in case another
 * sequence needs to be processed
 */
error_t shell_history_init(shell_t * sh);

/**
 * @brief Reset the history buffer
 */
error_t shell_history_clear(shell_t * sh);

/**
 * @brief Reset the history index
 */
error_t shell_history_reset_index(shell_t * sh);

/**
 * @brief Appends a line (command) to history
 *
 * @note Will reject empty lines
 * @note Will reject repeats (if previous entry == line)
 */
error_t shell_history_append(shell_t * sh, const char * line);

/**
 * @brief Get history line from buffer at specified index
 *
 * @return E_OK if found, E_EMPTY if no entry is available
 */
error_t shell_history_get_at(shell_t * sh, ssize_t index, char * line, size_t len);

/**
 * @brief Get history line from buffer at current history index
 *
 * @return E_OK if found, E_EMPTY if no entry is available
 */
error_t shell_history_get_current(shell_t * sh, char * line, size_t len);

/**
 * @brief Advance history buffer index
 */
error_t shell_history_advance(shell_t * sh);

/**
 * @brief Retard history buffer index
 */
error_t shell_history_retard(shell_t * sh);

#endif
