/** ========================================================================= *
 *
 * @file shell.c
 * @date 19-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Implementation of shell module
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <string.h>
#include "shell/shell.h"
#include "error/assertion.h"
#include "tty/ansi.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG      shell
#define SHELL_PROMPT "# " /* Shell prompt (TODO: make it into variable) */

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
extern void shell_parse(shell_t * sh);

__STATIC_INLINE void shell_reset_buffers(shell_t * sh) {
  tty_line_reset(&sh->line);

  memset(sh->args.buf, 0, sizeof(sh->args.buf));
  sh->args.size = 0;
}

/**
 * Prints prompt, and already typed line
 */
void shell_print_line(shell_t * sh, bool force) {
  bool write_detected = false;

  // Get write detected flag
  error_t err = vfs_ioctl(sh->tty.file, VFS_IOCTL_WRITE_DETECTED, &write_detected);

  // If ioctl is implemented for tty file & no writes were detected - don't
  // print prompt with command, because there is no need
  if (!force && (err == E_OK && !write_detected)) {
    return;
  }

  tty_line_t line;

  // Move cursor the beginning of the line
  tty_line_from_str(&line, ANSI_ERASE_FROM_CURSOR_TO_LINE_START "\r");
  tty_write_line(&sh->tty, &line);

  // If prompt is enabled - print it
  if (sh->flags & SHELL_FLAG_SHOWPROMPT) {
    tty_line_from_str(&line, SHELL_PROMPT);
    tty_write_line(&sh->tty, &line);
  }

  // If input line is not empty, print it
  if (sh->line.size) {
    tty_write_line(&sh->tty, &sh->line);
  }

  // Clear write detect flag
  vfs_ioctl(sh->tty.file, VFS_IOCTL_WRITE_DETECTED_CLEAR);
}

static error_t shell_exec(shell_t * sh, int8_t * result) {
  ASSERT_RETURN(sh && result, E_NULL);

#if USE_SHELL_ENV
  // Process every token and check for variable references
  for (uint8_t i = 0; i < sh->args.size; ++i) {
    sh->args.buf[i] = shell_arg_parse(sh, sh->args.buf[i]);
  }
#endif

  // Look for command handler by comparing name of each command to
  // the first token
  SHELL_ITER_COMMANDS(cmd) {
    if (!strcmp(sh->args.buf[0], cmd->name)) {
      *result = cmd->handler(sh, sh->args.size, sh->args.buf);

      if (sh->flags & SHELL_FLAG_ECHO_RES) {
        log_printf("=%d\r\n", *result);
      }

      return E_OK;
    }
  }

  log_error("Command '%s' not found", sh->args.buf[0]);

  *result = SHELL_FAIL;

  return E_NOTFOUND;
}

/* Shared functions ========================================================= */
error_t shell_init(shell_t * sh, vfs_file_t * file, void * handler_ctx) {
  ASSERT_RETURN(sh && file, E_NULL);

  memset(sh, 0, sizeof(shell_t));

#if USE_SHELL_HISTORY
  shell_history_clear(sh);
#endif

  tty_init(&sh->tty, file);

  sh->state = SHELL_STATE_IDLE;
  sh->flags = SHELL_FLAG_SHOWPROMPT;

  sh->commands.ctx = handler_ctx;

  return E_OK;
}

error_t shell_start(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);

  sh->state = SHELL_STATE_RUNNING;
  sh->internal_flags.is_new_line = true;

  shell_reset_buffers(sh);
  tty_reset(&sh->tty);

  log_printf("%s shell v%s\r\n", PROJECT_NAME, PROJECT_VERSION);

  return E_OK;
}

error_t shell_stop(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);

  sh->state = SHELL_STATE_IDLE;

  log_printf("exit\r\n\r\n");

  return E_OK;
}

bool shell_is_running(shell_t * sh) {
  ASSERT_RETURN(sh, false);
  return sh->state == SHELL_STATE_RUNNING;
}

error_t shell_process(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);

  if (sh->state != SHELL_STATE_RUNNING) {
    return E_INVAL;
  }

  if (sh->internal_flags.is_new_line) {
    sh->line.size = 0;
    sh->internal_flags.is_new_line = false;
  }

  shell_print_line(sh, false);

#if USE_SHELL_HISTORY
  shell_history_prepare_ansi_processor(sh);
#endif

  // When full line is received, this function returns E_OK, E_AGAIN otherwise
  ERROR_CHECK_RETURN(tty_read_line_async(&sh->tty, &sh->line));

  // If execution reached here, tty_read_line_async returned E_OK, and line
  // is present in the buffer
  sh->internal_flags.is_new_line = true;

#if USE_SHELL_HISTORY
  shell_history_append(sh, &sh->line);
  shell_history_reset(sh);
#endif

  shell_parse(sh);

  if (!sh->args.size || !strlen(sh->args.buf[0])) {
    return E_EMPTY;
  }

  int8_t result = SHELL_FAIL;

  shell_exec(sh, &result);

  shell_reset_buffers(sh);

  return result == SHELL_OK ? E_OK : E_FAILED;
}

int8_t shell_execute(shell_t * sh, const char * command) {
  ASSERT_RETURN(sh && command, E_NULL);

  sh->line.size = strlen(command);

  memcpy(sh->line.buf, command, sh->line.size);

  shell_parse(sh);

  int8_t result = SHELL_FAIL;

  ERROR_CHECK(shell_exec(sh, &result),
    return SHELL_FAIL);

  return result;
}
