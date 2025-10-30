/** ========================================================================= *
 *
 * @file line.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Implementation of TTY Line
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
/* Shared functions ========================================================= */
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

const char * tty_line_get_str(tty_line_t * line) {
  ASSERT_RETURN(line, NULL);

  line->buf[line->size] = '\0';

  return line->buf;
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
