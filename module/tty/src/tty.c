#include <tty.h>
#include <tty/ansi.h>
#include <tty/ascii.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#if WITH_TTY_EXTENDED_ANSI

/**
 * @brief Find & return a previous word boundary index in the input->buf
 *        relative to the cursor position
 */
__STATIC_INLINE uint16_t tty_find_prev_word_boundary(tty_input_t * input) {
  uint16_t index = input->cursor - 1;

  while (input->buf[index] != ' ') {
    index--;

    if (!index) {
      break;
    }
  }

  // Catch a case of erasing 2 spaces, leaving no space between prev prev word
  // and next word: "abc 123 def"
  //                         ^ cursor
  // Will turn into: "abcdef" instead of "abc def" without this
  if (input->buf[input->cursor] != ' ' && input->cursor - index > 1) {
    return index + 1;
  }

  return index;
}

/**
 * @brief Find & return next word boundary index in the input->buf
 *        relative to the cursor position
 */
__STATIC_INLINE uint16_t tty_find_next_word_boundary(tty_input_t * input) {
  uint16_t index = input->cursor + 1;

  while (input->buf[index] != ' ') {
    index++;

    if (index == input->size) {
      break;
    }
  }

  return index;
}

/**
 * @brief Erase previous word relative to the cursor position
 */
__STATIC_INLINE void tty_erase_word(tty_t * tty, tty_input_t * input) {
  if (!input->cursor || !input->size) {
    input->state = TTY_INPUT_STATE_DEFAULT;
    tty_bell(tty);
    return;
  }

  uint16_t word = tty_find_prev_word_boundary(input);

  if (input->cursor != input->size) {
    // Hard(er) - cursor is in the middle
    memcpy(
      &input->buf[word],
      &input->buf[input->cursor],
      input->size - word
    );
    input->size -= (input->cursor - word);
  } else {
    // Easy - cursor is at the end of the line
    input->size = word;
  }

  // Erase line to get rid of leftover (erased)
  // text and trigger a prompt re-print
  tty_puts(tty, ANSI_ERASE_LINE);

  input->cursor = word;
  input->state = TTY_INPUT_STATE_DEFAULT;
}

/**
 * @brief Move cursor to the previous word boundary
 *        relative to the cursor position
 */
__STATIC_INLINE void tty_move_to_left_word(tty_t * tty, tty_input_t * input) {
  if (!input->cursor || !input->size) {
    input->state = TTY_INPUT_STATE_DEFAULT;
    tty_bell(tty);
    return;
  }

  input->cursor = tty_find_prev_word_boundary(input);
  input->state = TTY_INPUT_STATE_DEFAULT;

  tty->flags |= TTY_FLAG_WRITE_DETECTION;
}

/**
 * @brief Move cursor to the next word boundary
 *        relative to the cursor position
 */
__STATIC_INLINE void tty_move_to_right_word(tty_t * tty, tty_input_t * input) {
  if (!input->size || input->cursor >= input->size) {
    input->state = TTY_INPUT_STATE_DEFAULT;
    tty_bell(tty);
    return;
  }

  input->cursor = tty_find_next_word_boundary(input);
  input->state = TTY_INPUT_STATE_DEFAULT;

  tty->flags |= TTY_FLAG_WRITE_DETECTION;
}

#endif

/**
 * @brief Processes an ANSI escape sequence start
 *
 * Called for a next char after a \x1b (ESC)
 */
static error_t tty_process_ansi_esc(tty_t * tty, tty_input_t * input, int c) {
  switch (c) {
    case '[': input->state = TTY_INPUT_STATE_CSI; break;
    case 'P': input->state = TTY_INPUT_STATE_DCS; break;
    case ']': input->state = TTY_INPUT_STATE_OSC; break;
#if WITH_TTY_EXTENDED_ANSI // TODO: Check for ESC b / ESC f in other terminals (mainly minicom)
    case 'b': tty_move_to_left_word(tty, input);  break;
    case 'f': tty_move_to_right_word(tty, input); break;
    case ASCII_BACKSPACE:
    case ASCII_DEL:
      tty_erase_word(tty, input);
      break;
#endif
    default: {
      input->state = TTY_INPUT_STATE_DEFAULT;
      break;
    }
  }

  return E_AGAIN;
}

/**
 * @brief Processes an ANSI CSI (Control Sequence Introducer) sequence
 *
 * Called for a next char after a \x1b (ESC) + [
 */
static error_t tty_process_ansi_csi(tty_t * tty, tty_input_t * input, int c) {
  switch (c) {
    case 'C': { // ESC [ C - Move cursor right
      if (input->cursor < input->size) {
        input->cursor++;
        tty_puts(tty, ANSI_CURSOR_MOVE_RIGHT());
      } else {
        tty_bell(tty);
      }
      break;
    }

    case 'D': { // ESC [ D - Move cursor left
      if (input->cursor > 0) {
        input->cursor--;
        tty_puts(tty, ANSI_CURSOR_MOVE_LEFT());
      } else {
        tty_bell(tty);
      }
      break;
    }

    default: {
      if (tty->input.custom.csi) {
        return tty->input.custom.csi(tty, input, c);
      }

      break;
    }
  }

  input->state = TTY_INPUT_STATE_DEFAULT;

  return E_AGAIN;
}

/**
 * @brief Processes an ANSI DCS (Device Control String) sequence
 *
 * Called for a next char after a \x1b (ESC) + P
 */
static error_t tty_process_ansi_dcs(tty_t * tty, tty_input_t * input, int c) {
  if (tty->input.custom.dcs) {
    return tty->input.custom.dcs(tty, input, c);
  }

  input->state = TTY_INPUT_STATE_DEFAULT;
  return E_AGAIN;
}

/**
 * @brief Processes an ANSI OSC (Operating System Command) sequence
 *
 * Called for a next char after a \x1b (ESC) + ]
 */
static error_t tty_process_ansi_osc(tty_t * tty, tty_input_t * input, int c) {
  if (tty->input.custom.osc) {
    return tty->input.custom.osc(tty, input, c);
  }

  input->state = TTY_INPUT_STATE_DEFAULT;
  return E_AGAIN;
}

/**
 * @brief Top-level ANSI sequence handler
 *
 * Basically a state machine for different escape sequences
 */
static error_t tty_process_ansi(tty_t * tty, tty_input_t * input, int c) {
  if (c == ASCII_ESCAPE) {
    input->state = TTY_INPUT_STATE_ESC;
    return E_AGAIN;
  }

  switch (input->state) {
    case TTY_INPUT_STATE_DEFAULT: return E_OK;
    case TTY_INPUT_STATE_ESC:     return tty_process_ansi_esc(tty, input, c);
    case TTY_INPUT_STATE_CSI:     return tty_process_ansi_csi(tty, input, c);
    case TTY_INPUT_STATE_DCS:     return tty_process_ansi_dcs(tty, input, c);
    case TTY_INPUT_STATE_OSC:     return tty_process_ansi_osc(tty, input, c);
    default:
      input->state = TTY_INPUT_STATE_DEFAULT;
      break;
  }

  return E_OK;
}

/**
 * @brief Process new character from input
 *
 * Handles backspace, enter, ANSI sequences, etc.
 */
static error_t tty_process_char(tty_t * tty, tty_input_t * input, int c) {
  // printf("\n%x '%c'\n", c, c);

  ERROR_CHECK_RETURN(tty_process_ansi(tty, input, c));

  if (c == ASCII_START_OF_HEADING) { // CTRL+A - move cursor to line start
    input->cursor = 0;
    tty->flags |= TTY_FLAG_WRITE_DETECTION;
    return E_AGAIN;
  }

  if (c == ASCII_ENQUIRY) { // CTRL+A - move cursor to line end
    input->cursor = input->size;
    tty->flags |= TTY_FLAG_WRITE_DETECTION;
    return E_AGAIN;
  }

  // If char is backspace (or delete)
  if (c == ASCII_BACKSPACE || c == ASCII_DEL) {
    // If string buffer is not empty
    if (input->size > 0 && input->cursor > 0) {
      if (input->cursor != input->size) {
        // If not at the end of the line (hard case)
        // Move the remainder of the line from char preceding cursor
        // (which will get erased) to the end of the line
        memcpy(
          &input->buf[input->cursor - 1],
          &input->buf[input->cursor],
          input->size - input->cursor + 1
        );

        // Move cursor to the left & reduce size by 1
        input->size   -= 1;
        input->cursor -= 1;

        // Reprint remainder of line (from cursor to the end) and move
        // cursor back to where it belongs
        if (tty->flags & TTY_FLAG_ECHO) {
          size_t left = input->size - input->cursor;

          tty_printf(
            tty,
            "\b%.*s " ANSI_CURSOR_MOVE_LEFT_FMT,
            (int)left, &input->buf[input->cursor], (int)(left + 1)
          );
        }
      } else {
        // If at the end of the line (easy case)
        // Send backspace, which will move the cursor left by 1 char, then
        // send blank space to overwrite the symbol and another backspace
        // to put cursor 1 symbol back
        if (tty->flags & TTY_FLAG_ECHO) {
          char buf[3] = {ASCII_BACKSPACE, ' ', ASCII_BACKSPACE};
          tty_write(tty, buf, 3);
        }

        // Move cursor to the left & reduce size by 1
        input->size   -= 1;
        input->cursor -= 1;
      }
    } else {
      tty_bell(tty);
    }

    // If there is nothing to erase, skip
    return E_AGAIN;
  }

  // If char is enter (CR), send CR & LF back into the UART
  // TODO: How to make it so that any combination works?
  //       Maybe disallow \r as the first char in the line???
  if (c == ASCII_LINE_FEED || c == ASCII_CARRIAGE_RETURN) {
    if (tty->flags & TTY_FLAG_ECHO) {
      char buf[2] = {'\r', '\n'};
      tty_write(tty, buf, 2);
    }

    // Terminate the buffer
    input->buf[input->size] = '\0';
    return E_OK;
  }

  if (input->cursor != input->size) {
    // If cursor is not at the end of the line (hard case)
    // Move remainder chars from the end to the cursor to the left
    for (ssize_t i = (ssize_t)input->size - input->cursor + 1; i >= 0; --i) {
      input->buf[input->cursor + i + 1] = input->buf[input->cursor + i];
    }

    // Save char at cursor position
    input->buf[input->cursor++] = (char)c;
    input->size++;

    if (tty->flags & TTY_FLAG_ECHO) {
      size_t left = input->size - input->cursor;

      // Print new char + remainder of line (from cursor to the end) and move
      // cursor back to where it belongs
      tty_printf(
        tty,
        "%.*s" ANSI_CURSOR_MOVE_LEFT_FMT,
        (int)(left + 1), &input->buf[input->cursor - 1], (int)left
      );
    }
  } else {
    // If cursor is at the end of the line (easy case)
    // Save char to the end of the buffer
    input->buf[input->size++] = c;
    input->cursor++;

    // If input echoing is on, send received char back unto UART to display it
    if (tty->flags & TTY_FLAG_ECHO) {
      tty_write(tty, &(char) { (char) c }, 1);
    }
  }

  return E_AGAIN;
}

error_t tty_input_reset(tty_input_t * input) {
  ASSERT_RETURN(input, E_NULL);
  
  input->buf = NULL;
  input->size = 0;
  input->cursor = 0;

  return E_OK;
}

error_t tty_input_clear(tty_input_t * input) {
  ASSERT_RETURN(input, E_NULL);

  // input->buf = NULL;
  input->size = 0;
  input->cursor = 0;

  return E_OK;
}

error_t tty_init(tty_t * tty, device_t * dev) {
  ASSERT_RETURN(tty && dev, E_NULL);

  memset(tty, 0, sizeof(tty_t));

  tty->dev = dev;

  return E_OK;
}

error_t tty_read(tty_t * tty, char * buf, size_t len) {
  ASSERT_RETURN(tty, E_NULL);
  ASSERT_RETURN(len, E_EMPTY);

  return device_read(tty->dev, (uint8_t *)buf, len);
}

error_t tty_readline(tty_t * tty, char * buf, size_t len) {
  ASSERT_RETURN(tty && buf, E_NULL);

  tty->input.buf = buf;

  while (tty->input.size < len) {
    char c = '\0';

    error_t err = tty_read(tty, &c, 1);

    if (err == E_AGAIN || c == '\0') {
      return E_AGAIN;
    }

    if (tty_process_char(tty, &tty->input, c) == E_OK) {
      tty_input_reset(&tty->input);
      return E_OK;
    }
  }

  tty->input.buf[tty->input.size] = '\0';
  tty_input_reset(&tty->input);

  return E_OK;
}

error_t tty_write(tty_t * tty, const char * buf, size_t len) {
  ASSERT_RETURN(tty && tty->dev, E_NULL);
  ASSERT_RETURN(len, E_EMPTY);

  tty->flags |= TTY_FLAG_WRITE_DETECTION;

  return device_write(tty->dev, (const uint8_t *)buf, len);
}

error_t tty_puts(tty_t * tty, const char * str) {
  return tty_write(tty, str, strlen(str));
}

error_t tty_vprintf(tty_t * tty, const char * fmt, va_list ap) {
  ASSERT_RETURN(tty && fmt, E_NULL);

  char buffer[TTY_MAX_LINE_SIZE];

  size_t size = vsnprintf(buffer, sizeof(buffer), fmt, ap);
  return tty_write(tty, buffer, size);
}

error_t tty_printf(tty_t * tty, const char * fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  error_t res = tty_vprintf(tty, fmt, ap);

  va_end(ap);

  return res;
}

error_t tty_set_blocking(tty_t * tty, bool blocking) {
  ASSERT_RETURN(tty, E_NULL);
  ASSERT_RETURN(device_supports_ioctl(tty->dev, DEV_IOCTL_SET_BLOCKING), E_NOTIMPL);

  return device_ioctl(tty->dev, DEV_IOCTL_SET_BLOCKING, blocking);
}

error_t tty_bell(tty_t * tty) {
  ASSERT_RETURN(tty, E_NULL);

  if (tty->flags & TTY_FLAG_BELL) {
    tty_write(tty, &(char){ASCII_BELL}, 1);
  }

  return E_OK;
}

#if WITH_TTY_COLORTEST
void tty_colortest(tty_t * tty) {
  for (int row = 0; row < 11; row++) {
    for (int col = 0; col < 10; col++) {
      int n = 10 * row + col;

      if (n > 109) {
        break;
      }

      tty_printf(tty, "\x1b[%dm %3d\x1b[m", n, n);
    }

    tty_puts(tty, "\n");
  }
}
#endif
