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
#include "error/assertion.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
error_t tty_init(tty_t * tty, vfs_file_t * file) {
  ASSERT_RETURN(tty && file, E_NULL);

  tty->file = file;
  tty->flags = TTY_FLAG_ECHO_INPUT;

  return E_OK;
}

error_t tty_reset(tty_t * tty) {
  ASSERT_RETURN(tty, E_NULL);

  return vfs_ioctl(tty->file, VFS_IOCTL_RESET_DEVICE);
}

error_t tty_line_from_str(tty_line_t * line, const char * str) {
  ASSERT_RETURN(line && str, E_NULL);

  line->size = strlen(str);
  memcpy(line->buf, str, line->size + 1);

  return E_OK;
}

error_t tty_line_from_buf(tty_line_t * line, const char * buf, size_t size) {
  ASSERT_RETURN(line && buf, E_NULL);

  line->size = size;
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

error_t tty_read_line(tty_t * tty, tty_line_t * line) {
  ASSERT_RETURN(tty && line, E_NULL);

  line->size = 0;

  while (line->size < TTY_MAX_LINE_SIZE) {
    char c = '\0';

    // Read 1 char from UART
    vfs_read(tty->file, (uint8_t *) &c, 1, VFS_READ_FLAG_NOBLOCK);

    if (c == TTY_ASCII_KEY_ESC) {
      continue;
    }

    // If char is backspace
    if (c == TTY_ASCII_KEY_BACKSPACE) {
      // If string buffer is not empty
      if (line->size > 0) {
        // Send backspace, which will move the cursor left by 1 char, then
        // send blank space to overwrite the symbol and another backspace
        // to put cursor 1 symbol back
        char buf[3] = {c, ' ', c};
        vfs_write(tty->file, (uint8_t *) buf, 3);
        line->size -= 1;
      }
      // If there is nothing to erase, skip
      continue;
    }

    // If char is enter (CR), send CR & LF back into the UART
    if (c == TTY_ASCII_KEY_ENTER) {
      char buf[2] = {'\r', '\n'};
      vfs_write(tty->file, (uint8_t *) buf, 2);
      line->size += 1;
      break;
    }

    // Save char to the buffer
    line->buf[line->size++] = c;

    // If input echoing is on, send received char back unto UART to display it
    if (tty_get_flag(tty, TTY_FLAG_ECHO_INPUT)) {
      vfs_write(tty->file, (uint8_t *) &c, 1);
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

    // Read 1 char from UART
    ERROR_CHECK_RETURN(vfs_read(tty->file, (uint8_t *) &c, 1, VFS_READ_FLAG_NONE));

    if (c == TTY_ASCII_KEY_ESC) {
      return E_AGAIN;
    }

    // If char is backspace
    if (c == TTY_ASCII_KEY_BACKSPACE) {
      // If string buffer is not empty
      if (line->size > 0) {
        // Send backspace, which will move the cursor left by 1 char, then
        // send blank space to overwrite the symbol and another backspace
        // to put cursor 1 symbol back
        char buf[3] = {c, ' ', c};
        vfs_write(tty->file, (uint8_t *) buf, 3);
        line->size -= 1;
      }
      // If there is nothing to erase, skip
      return E_AGAIN;
    }

    // If char is enter (CR), send CR & LF back into the UART
    if (c == TTY_ASCII_KEY_ENTER) {
      char buf[2] = {'\r', '\n'};
      vfs_write(tty->file, (uint8_t *) buf, 2);
      line->size += 1;

      // Terminate the buffer
      line->buf[line->size-1] = '\0';
      return E_OK;
    }

    // Save char to the buffer
    line->buf[line->size++] = c;

    // If input echoing is on, send received char back unto UART to display it
    if (tty_get_flag(tty, TTY_FLAG_ECHO_INPUT)) {
      vfs_write(tty->file, (uint8_t *) &c, 1);
    }

    return E_AGAIN;
  } else {
    // Terminate the buffer
    line->buf[line->size-1] = '\0';
    return E_OK;
  }
}

error_t tty_write_line(tty_t * tty, tty_line_t * line) {
  ASSERT_RETURN(tty && line, E_NULL);

  return vfs_write(tty->file, (uint8_t *) line->buf, line->size);
}

