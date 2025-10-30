/** ========================================================================= *
 *
 * @file shell_history.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Implementation of shell history
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG shell

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
#if USE_SHELL_HISTORY
static shell_t * sh_history_ctx = NULL;
#endif

/* Private functions ======================================================== */
extern void shell_print_line(shell_t * sh, bool force);

/* Shared functions ========================================================= */
#if USE_SHELL_HISTORY
error_t shell_history_clear(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);

  memset(sh->history.buffer, 0, sizeof(sh->history.buffer));

  // Default value for index & head is -1
  sh->history.index = -1;
  sh->history.head  = -1;

  return E_OK;
}

error_t shell_history_reset(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);

  sh->history.index = -1;

  return E_OK;
}

error_t shell_history_append(shell_t * sh, tty_line_t * line) {
  ASSERT_RETURN(sh && line,   E_NULL);
  ASSERT_RETURN(line->size,   E_EMPTY); // Check if line isn't empty
  ASSERT_RETURN(line->buf[0], E_EMPTY); // Check again for null terminator, just to be sure

  if (sh->history.head == -1) {
    // If no entries were present before - set index to 0, as the first entry
    sh->history.head = 0;
  } else {
    // Check for duplicates
    if (!strcmp(sh->history.buffer[sh->history.head].line, tty_line_get_str(line))) {
      return E_AGAIN;
    }

    // Increment pointer to head and wrap if overflowed
    sh->history.head = (sh->history.head + 1) % SHELL_HISTORY_BUFFER_SIZE;
  }

  // Save command
  memcpy(sh->history.buffer[sh->history.head].line, tty_line_get_str(line), line->size + 1);

  return E_OK;
}

error_t shell_history_get(shell_t * sh, tty_line_t * line) {
  ASSERT_RETURN(sh && line, E_NULL);

  // If no history is present - erase line
  if (sh->history.index == -1) {
    tty_line_reset(&sh_history_ctx->line);
    return E_OK;
  }

  // Check if not empty
  ASSERT_RETURN(sh->history.buffer[sh->history.index].line[0], E_EMPTY);

  // Copy from buffer to line
  tty_line_from_str(line, sh->history.buffer[sh->history.index].line);

  return E_OK;
}

error_t shell_history_advance(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);
  ASSERT_RETURN(sh->history.head != -1, E_EMPTY); // Check if at least 1 entry is present

  ssize_t index = sh->history.index;

  // If this is the first time advance was called - set index to last command entry
  if (index == -1) {
    sh->history.index = sh->history.head;
    return E_OK;
  }

  // Decrement and wrap if under flowed
  if (index == 0) {
    index = SHELL_HISTORY_BUFFER_SIZE - 1;
  } else {
    index -= 1;
  }

  // If reached first command again - return
  if (index == sh->history.head) {
    return E_OUTOFBOUNDS;
  }

  // Check if entry is not empty
  ASSERT_RETURN(sh->history.buffer[index].line[0], E_EMPTY);

  // Save calculated index
  sh->history.index = index;

  return E_OK;
}

error_t shell_history_retard(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);
  ASSERT_RETURN(sh->history.head != -1, E_EMPTY); // Check if at least 1 entry is present
  ASSERT_RETURN(sh->history.buffer[sh->history.index].line[0], E_EMPTY); // Check if not empty

  // If reached the first command - reset index
  if (sh->history.index == sh->history.head) {
    sh->history.index = -1;
    return E_OK;
  }

  // Increment pointer to head and wrap if overflowed
  sh->history.index = (sh->history.index + 1) % SHELL_HISTORY_BUFFER_SIZE;

  return E_OK;
}

void shell_history_prepare_ansi_processor(shell_t * sh) {
  // Save current shell context to global variable
  sh_history_ctx = sh;
}

error_t shell_history_process_ansi_csi(tty_t * tty, tty_line_t * line, char c) {
  switch (c) {
    case 'A': { // ESC [ A - Move cursor up
      if (sh_history_ctx) {
        if (shell_history_advance(sh_history_ctx) == E_OK) {
          // If context is present, and index is moved successfully -
          // reprint line with command from history
          shell_history_get(sh_history_ctx, &sh_history_ctx->line);
          shell_print_line(sh_history_ctx, true);
          break;
        }
      }

      tty_bell(tty);
      break;
    }

    case 'B': { // ESC [ B - Move cursor down
      if (sh_history_ctx) {
        if (shell_history_retard(sh_history_ctx) == E_OK) {
          // If context is present, and index is moved successfully -
          // reprint line with command from history
          shell_history_get(sh_history_ctx, &sh_history_ctx->line);
          shell_print_line(sh_history_ctx, true);
          break;
        }
      }

      tty_bell(tty);
      break;
    }

    default:
      break;
  }

  line->input.state = TTY_LINE_INPUT_STATE_DEFAULT;
  return E_AGAIN;
}
#endif
