/** ========================================================================= *
 *
 * @file tty.c
 * @date 09-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Implementation of TTY Emulation functionality
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "tty/tty.h"
#include "ansi.h"
#include "error/assertion.h"
#include <stdio.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
static error_t tty_process_ansi_esc(__UNUSED tty_t * tty, tty_line_t * line, char c) {
  switch (c) {
    case '[': line->input.state = TTY_LINE_INPUT_STATE_CSI;     break;
    case 'P': line->input.state = TTY_LINE_INPUT_STATE_DCS;     break;
    case ']': line->input.state = TTY_LINE_INPUT_STATE_OSC;     break;
    default:  line->input.state = TTY_LINE_INPUT_STATE_DEFAULT; break;
  }
  return E_AGAIN;
}

static error_t tty_process_ansi_csi(tty_t * tty, tty_line_t * line, char c) {
  switch (c) {
    case 'C': // ESC [ C - Move cursor right
      if (line->input.cursor < line->size) {
        line->input.cursor++;
        tty_write_fmt(tty, ANSI_CURSOR_MOVE_RIGHT());
      } else {
        tty_bell(tty);
      }
      break;

    case 'D': // ESC [ D - Move cursor left
      if (line->input.cursor > 0) {
        line->input.cursor--;
        tty_write_fmt(tty, ANSI_CURSOR_MOVE_LEFT());
      } else {
        tty_bell(tty);
      }
      break;

    default:
      return tty_process_ansi_csi_custom(tty, line, c);
  }

  line->input.state = TTY_LINE_INPUT_STATE_DEFAULT;

  return E_AGAIN;
}

static error_t tty_process_ansi_dcs(tty_t * tty, tty_line_t * line, char c) {
  return tty_process_ansi_dcs_custom(tty, line, c);
}

static error_t tty_process_ansi_osc(tty_t * tty, tty_line_t * line, char c) {
  return tty_process_ansi_osc_custom(tty, line, c);
}

static error_t tty_process_ansi(tty_t * tty, tty_line_t * line, char c) {
  if (c == TTY_ASCII_KEY_ESC) {
    line->input.state = TTY_LINE_INPUT_STATE_ESC;
    return E_AGAIN;
  }

  switch (line->input.state) {
    case TTY_LINE_INPUT_STATE_DEFAULT: return E_OK;
    case TTY_LINE_INPUT_STATE_ESC:     return tty_process_ansi_esc(tty, line, c);
    case TTY_LINE_INPUT_STATE_CSI:     return tty_process_ansi_csi(tty, line, c);
    case TTY_LINE_INPUT_STATE_DCS:     return tty_process_ansi_dcs(tty, line, c);
    case TTY_LINE_INPUT_STATE_OSC:     return tty_process_ansi_osc(tty, line, c);
    default:
      line->input.state = TTY_LINE_INPUT_STATE_DEFAULT;
      break;
  }

  return E_OK;
}

static error_t tty_process_char(tty_t * tty, tty_line_t * line, char c) {
  ERROR_CHECK_RETURN(tty_process_ansi(tty, line, c));

  // If char is backspace
  if (c == TTY_ASCII_KEY_BACKSPACE) {
    // If string buffer is not empty
    if (line->size > 0 && line->input.cursor > 0) {
      if (line->input.cursor != line->size) {
        // If not at the end of the line (hard case)
        // Move the remainder of the line from char preceding cursor
        // (which will get erased) to the end of the line
        memcpy(
          &line->buf[line->input.cursor - 1],
          &line->buf[line->input.cursor],
          line->size - line->input.cursor + 1
        );

        // Move cursor to the left & reduce size by 1
        line->size         -= 1;
        line->input.cursor -= 1;


        // Reprint remainder of line (from cursor to the end) and move
        // cursor back to where it belongs
        if (tty_get_flag(tty, TTY_FLAG_ECHO_INPUT)) {
          size_t left = line->size - line->input.cursor;

          tty_write_fmt(
            tty,
            "\b%.*s " ANSI_CURSOR_MOVE_LEFT_FMT,
            left, &line->buf[line->input.cursor], left + 1
          );
        }
      } else {
        // If at the end of the line (easy case)
        // Send backspace, which will move the cursor left by 1 char, then
        // send blank space to overwrite the symbol and another backspace
        // to put cursor 1 symbol back
        if (tty_get_flag(tty, TTY_FLAG_ECHO_INPUT)) {
          char buf[3] = {c, ' ', c};
          vfs_write(tty->file, (uint8_t *) buf, 3);
        }

        // Move cursor to the left & reduce size by 1
        line->size         -= 1;
        line->input.cursor -= 1;
      }
    } else {
      tty_bell(tty);
    }

    // If there is nothing to erase, skip
    return E_AGAIN;
  }

  // If char is enter (CR), send CR & LF back into the UART
  if (c == TTY_ASCII_KEY_ENTER) {
    if (tty_get_flag(tty, TTY_FLAG_ECHO_INPUT)) {
      char buf[2] = {'\r', '\n'};
      vfs_write(tty->file, (uint8_t *) buf, 2);
    }

    line->size += 1;

    // Terminate the buffer
    line->buf[line->size-1] = '\0';
    return E_OK;
  }

  if (line->input.cursor != line->size) {
    // If cursor is not at the end of the line (hard case)
    // Move remainder chars from the end to the cursor to the left
    for (ssize_t i = line->size - line->input.cursor + 1; i >= 0; --i) {
      line->buf[line->input.cursor + i + 1] = line->buf[line->input.cursor + i];
    }

    // Save char at cursor position
    line->buf[line->input.cursor++] = c;
    line->size++;

    if (tty_get_flag(tty, TTY_FLAG_ECHO_INPUT)) {
      size_t left = line->size - line->input.cursor;

      // Print new char + remainder of line (from cursor to the end) and move
      // cursor back to where it belongs
      tty_write_fmt(
        tty,
        "%.*s" ANSI_CURSOR_MOVE_LEFT_FMT,
        left + 1, &line->buf[line->input.cursor - 1], left
      );
    }
  } else {
    // If cursor is at the end of the line (easy case)
    // Save char to the end of the buffer
    line->buf[line->size++] = c;
    line->input.cursor++;

    // If input echoing is on, send received char back unto UART to display it
    if (tty_get_flag(tty, TTY_FLAG_ECHO_INPUT)) {
      vfs_write(tty->file, (uint8_t *) &c, 1);
    }
  }

  return E_AGAIN;
}

/* Shared functions ========================================================= */
error_t tty_init(tty_t * tty, vfs_file_t * file) {
  ASSERT_RETURN(tty && file, E_NULL);

  tty->file = file;
  tty->flags = TTY_FLAG_ECHO_INPUT | TTY_FLAG_USE_BELL;

  return E_OK;
}

error_t tty_reset(tty_t * tty) {
  ASSERT_RETURN(tty, E_NULL);

  return vfs_ioctl(tty->file, VFS_IOCTL_RESET_DEVICE);
}

error_t tty_line_reset(tty_line_t * line) {
  ASSERT_RETURN(line, E_NULL);

  line->size               = 0;
  line->input.cursor       = 0;
  line->input.state        = TTY_LINE_INPUT_STATE_DEFAULT;
  line->input.custom_state = TTY_LINE_INPUT_CUSTOM_STATE_DEFAULT;

  memset(line->buf, 0, sizeof(line->buf));

  return E_OK;
}

error_t tty_line_from_str(tty_line_t * line, const char * str) {
  ASSERT_RETURN(line && str, E_NULL);

  line->input.state        = TTY_LINE_INPUT_STATE_DEFAULT;
  line->input.custom_state = TTY_LINE_INPUT_CUSTOM_STATE_DEFAULT;
  line->size               = strlen(str);
  line->input.cursor       = line->size;

  memcpy(line->buf, str, line->size + 1);

  return E_OK;
}

error_t tty_line_from_buf(tty_line_t * line, const char * buf, size_t size) {
  ASSERT_RETURN(line && buf, E_NULL);

  line->input.state        = TTY_LINE_INPUT_STATE_DEFAULT;
  line->input.custom_state = TTY_LINE_INPUT_CUSTOM_STATE_DEFAULT;
  line->size               = size;
  line->input.cursor       = size;

  memcpy(line->buf, buf, size + 1);

  return E_OK;
}

error_t tty_set_flag(tty_t * tty, tty_flag_t flag) {
  ASSERT_RETURN(tty, E_NULL);

  tty->flags |= flag;

  return E_OK;
}

error_t tty_clear_flag(tty_t * tty, tty_flag_t flag) {
  ASSERT_RETURN(tty, E_NULL);

  tty->flags &= ~flag;

  return E_OK;
}

bool tty_get_flag(tty_t * tty, tty_flag_t flag) {
  ASSERT_RETURN(tty, false);

  return tty->flags & flag ? 1 : 0;
}

error_t tty_get_char(tty_t * tty, char * ch) {
  ASSERT_RETURN(tty && ch, E_NULL);

  return vfs_read(tty->file, (uint8_t *) &ch, 1, VFS_READ_FLAG_NONE);
}

error_t tty_get_char_async(tty_t * tty, char * ch) {
  ASSERT_RETURN(tty && ch, E_NULL);

  return vfs_read(tty->file, (uint8_t *) &ch, 1, VFS_READ_FLAG_NOBLOCK);
}

error_t tty_read_line(tty_t * tty, tty_line_t * line) {
  ASSERT_RETURN(tty && line, E_NULL);

  line->size = 0;

  while (line->size < TTY_MAX_LINE_SIZE) {
    char c = '\0';

    // Read 1 char from UART
    if (vfs_read(tty->file, (uint8_t *) &c, 1, VFS_READ_FLAG_NONE) == E_OK) {
      if (tty_process_char(tty, line, c) == E_OK) {
        return E_OK;
      }
    }
  }

  // Terminate the buffer
  line->buf[line->size-1] = '\0';

  return E_OK;
}

error_t tty_read_line_async(tty_t * tty, tty_line_t * line) {
  ASSERT_RETURN(tty && line, E_NULL);

  if (line->size < TTY_MAX_LINE_SIZE) {
    char c = '\0';

    if (!vfs_tell(tty->file)) {
      return E_AGAIN;
    }

    ERROR_CHECK_RETURN(vfs_read(tty->file, (uint8_t *) &c, 1, VFS_READ_FLAG_NONE));

    return tty_process_char(tty, line, c);
  }

  // Terminate the buffer
  line->buf[line->size-1] = '\0';

  return E_OK;
}

error_t tty_write_line(tty_t * tty, tty_line_t * line) {
  ASSERT_RETURN(tty && line, E_NULL);

  ERROR_CHECK_RETURN(vfs_write(tty->file, (uint8_t *) line->buf, line->size));

  // If cursor is not at the end of line
  if (line->input.cursor < line->size) {
    // Move the cursor left, because whole line was just printed, and cursor is
    // currently at the end of it
    tty_write_fmt(tty, ANSI_CURSOR_MOVE_LEFT_FMT, line->size - line->input.cursor);
  }

  return E_OK;
}

error_t tty_write_fmt(tty_t * tty, const char * fmt, ...) {
  ASSERT_RETURN(tty && fmt, E_NULL);

  va_list args;
  va_start(args, fmt);

  char buffer[TTY_MAX_LINE_SIZE];
  size_t size = vsnprintf(buffer, sizeof(buffer), fmt, args);
  error_t result = vfs_write(tty->file, (uint8_t *) buffer, size);

  va_end(args);

  return result;
}

error_t tty_bell(tty_t * tty) {
  ASSERT_RETURN(tty);

  if (tty_get_flag(tty, TTY_FLAG_USE_BELL)) {
    vfs_write(tty->file, (uint8_t *) &(char){TTY_ASCII_KEY_BEL}, 1);
  }

  return E_OK;
}

__WEAK error_t tty_process_ansi_csi_custom(__UNUSED tty_t * tty, tty_line_t * line, __UNUSED char c) {
  line->input.state = TTY_LINE_INPUT_STATE_DEFAULT;
  return E_AGAIN;
}

__WEAK error_t tty_process_ansi_dcs_custom(__UNUSED tty_t * tty, tty_line_t * line, __UNUSED char c) {
  line->input.state = TTY_LINE_INPUT_STATE_DEFAULT;
  return E_AGAIN;
}

__WEAK error_t tty_process_ansi_osc_custom(__UNUSED tty_t * tty, tty_line_t * line, __UNUSED char c) {
  line->input.state = TTY_LINE_INPUT_STATE_DEFAULT;
  return E_AGAIN;
}
