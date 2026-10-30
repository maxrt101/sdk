#include <shell.h>
#include <util.h>
#include <string.h>

#if WITH_SHELL_HISTORY

void shell_print_line(shell_t * sh, bool force);

static void shell_history_entry_to_tty_input(shell_t * sh) {
  // If context is present (checked in switch-case), and index is
  // moved successfully - reprint line with command from history
  if (shell_history_get_current(sh, sh->tty->input.buf, sizeof(sh->buf)) == E_OK) {
    sh->tty->input.size = strlen(sh->tty->input.buf);
    sh->tty->input.cursor = sh->tty->input.size;
  } else {
    tty_input_clear(&sh->tty->input);
  }
  shell_print_line(sh, true);
}

static error_t shell_history_process_ansi_csi(tty_t * tty, tty_input_t * input, int c) {
  shell_t * sh = tty->input.ctx.csi;

  switch (c) {
    case 'A': { // ESC [ A - Move cursor up
      if (sh && shell_history_advance(sh) == E_OK) {
        shell_history_entry_to_tty_input(sh);
        break;
      }

      tty_bell(tty);
      break;
    }

    case 'B': { // ESC [ B - Move cursor down
      if (sh && shell_history_retard(sh) == E_OK) {
        shell_history_entry_to_tty_input(sh);
        break;
      }

      tty_bell(tty);
      break;
    }

    default:
      break;
  }

  if (sh && sh->history.prev_csi_handler) {
    tty->input.ctx.csi = sh->history.prev_csi_context;
    error_t res = sh->history.prev_csi_handler(tty, input, c);
    tty->input.ctx.csi = sh;
    return res;
  }

  input->state = TTY_INPUT_STATE_DEFAULT;
  return E_AGAIN;
}

error_t shell_history_init(shell_t * sh) {
  ASSERT_RETURN(sh && sh->tty, E_NULL);

  sh->history.prev_csi_handler = sh->tty->input.custom.csi;
  sh->history.prev_csi_context = sh->tty->input.ctx.csi;

  sh->tty->input.custom.csi = shell_history_process_ansi_csi;
  sh->tty->input.ctx.csi = sh;

  return E_OK;
}

error_t shell_history_clear(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);

  memset(sh->history.buffer, 0, sizeof(sh->history.buffer));

  // Default value for index & head is -1
  sh->history.index = -1;
  sh->history.head  = -1;

  return E_OK;
}

error_t shell_history_reset_index(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);

  sh->history.index = -1;

  return E_OK;
}

error_t shell_history_append(shell_t * sh, const char * line) {
  ASSERT_RETURN(sh && line, E_NULL);
  ASSERT_RETURN(line[0],    E_EMPTY);

  if (sh->history.head == -1) {
    // If no entries were present before - set index to 0, as the first entry
    sh->history.head = 0;
  } else {
    // Check for duplicates
    if (!strcmp(sh->history.buffer[sh->history.head].line, line)) {
      return E_ALREADY;
    }

    // Increment pointer to head and wrap if overflowed
    sh->history.head = (sh->history.head + 1) % SHELL_HISTORY_BUFFER_SIZE;
  }

  // Save command
  memcpy(sh->history.buffer[sh->history.head].line, line, strlen(line) + 1);

  return E_OK;
}

error_t shell_history_get_at(shell_t * sh, ssize_t index, char * line, size_t len) {
  ASSERT_RETURN(sh && line, E_NULL);

  // If no history is present - return an error to erase line
  if (index == -1) {
    return E_EMPTY;
  }

  // Check if not empty
  ASSERT_RETURN(sh->history.buffer[index].line[0], E_EMPTY);

  // Copy from buffer to line
  // tty_line_from_str(line, sh->history.buffer[index].line);
  memcpy(
    (void *)line,
    sh->history.buffer[index].line,
    UTIL_MIN(strlen(sh->history.buffer[index].line), len) + 1
  );

  return E_OK;
}

error_t shell_history_get_current(shell_t * sh, char * line, size_t len) {
  return shell_history_get_at(sh, sh->history.index, line, len);
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

#endif
