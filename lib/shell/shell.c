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
extern void shell_parse(shell_t * ctx);

__STATIC_INLINE void shell_reset_buffers(shell_t * ctx) {
  memset(ctx->line.buf, 0, sizeof(ctx->line.buf));
  ctx->line.size = 0;

  memset(ctx->ptr.buf, 0, sizeof(ctx->ptr.buf));
  ctx->ptr.size = 0;
}

/**
 * Prints prompt, and already typed line
 */
__STATIC_INLINE void shell_print_line(shell_t * ctx) {
  bool write_detected = false;

  // Get write detected flag
  error_t err = vfs_ioctl(ctx->tty.file, VFS_IOCTL_WRITE_DETECTED, &write_detected);

  // If ioctl is implemented for tty file & no writes were detected - don't
  // print prompt with command, because there is no need
  if (err == E_OK && !write_detected) {
    return;
  }

  tty_line_t line;

  // Move cursor the beginning of the line
  tty_line_from_str(&line, "\r");
  tty_write_line(&ctx->tty, &line);

  // If prompt is enabled - print it
  if (ctx->flags & SHELL_FLAG_SHOWPROMPT) {
    tty_line_from_str(&line, SHELL_PROMPT);
    tty_write_line(&ctx->tty, &line);
  }

  // If input line is not empty, print it
  if (ctx->line.size) {
    tty_write_line(&ctx->tty, &ctx->line);
  }

  // Clear write detect flag
  vfs_ioctl(ctx->tty.file, VFS_IOCTL_WRITE_DETECTED_CLEAR);
}

static error_t shell_exec(shell_t * ctx, int8_t * result) {
  ASSERT_RETURN(ctx && result, E_NULL);

#if USE_SHELL_ENV
  // Process every token and check for variable references
  for (uint8_t i = 0; i < ctx->ptr.size; ++i) {
    ctx->ptr.buf[i] = shell_arg_parse(ctx, ctx->ptr.buf[i]);
  }
#endif

  // Look for command handler by comparing name of each command to
  // the first token
  SHELL_ITER_COMMANDS(cmd) {
    if (!strcmp(ctx->ptr.buf[0], cmd->name)) {
      *result = cmd->handler(ctx, ctx->ptr.size, ctx->ptr.buf);

      if (ctx->flags & SHELL_FLAG_ECHO_RES) {
        log_printf("=%d\r\n", *result);
      }

      return E_OK;
    }
  }

  log_error("Command '%s' not found", ctx->ptr.buf[0]);

  *result = SHELL_FAIL;

  return E_NOTFOUND;
}

/* Shared functions ========================================================= */
error_t shell_init(shell_t * ctx, vfs_file_t * file, void * handler_ctx) {
  ASSERT_RETURN(ctx && file, E_NULL);

  memset(ctx, 0, sizeof(shell_t));

  tty_init(&ctx->tty, file);

  ctx->state = SHELL_STATE_IDLE;
  ctx->flags = SHELL_FLAG_SHOWPROMPT;

  ctx->commands.ctx = handler_ctx;

  return E_OK;
}

error_t shell_start(shell_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  ctx->state = SHELL_STATE_RUNNING;
  ctx->internal_flags.is_new_line = true;

  shell_reset_buffers(ctx);
  tty_reset(&ctx->tty);

  log_printf("%s shell v%s\r\n", PROJECT_NAME, PROJECT_VERSION);

  return E_OK;
}

error_t shell_stop(shell_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  ctx->state = SHELL_STATE_IDLE;

  log_printf("exit\r\n\r\n");

  return E_OK;
}

bool shell_is_running(shell_t * ctx) {
  ASSERT_RETURN(ctx, false);
  return ctx->state == SHELL_STATE_RUNNING;
}

error_t shell_process(shell_t * ctx) {
  ASSERT_RETURN(ctx, E_NULL);

  if (ctx->state != SHELL_STATE_RUNNING) {
    return E_INVAL;
  }

  if (ctx->internal_flags.is_new_line) {
    ctx->line.size = 0;
    ctx->internal_flags.is_new_line = false;
  }

  shell_print_line(ctx);

  // When full line is received, this function returns E_OK, E_AGAIN otherwise
  ERROR_CHECK_RETURN(tty_read_line_async(&ctx->tty, &ctx->line));

  // If execution reached here, tty_read_line_async returned E_OK, and line
  // is present in the buffer
  ctx->internal_flags.is_new_line = true;

  shell_parse(ctx);

  if (!ctx->ptr.size || !strlen(ctx->ptr.buf[0])) {
    return E_EMPTY;
  }

  int8_t result = SHELL_FAIL;

  shell_exec(ctx, &result);

  shell_reset_buffers(ctx);

  return result == SHELL_OK ? E_OK : E_FAILED;
}

int8_t shell_execute(shell_t * ctx, const char * command) {
  ASSERT_RETURN(ctx && command, E_NULL);

  ctx->line.size = strlen(command);

  memcpy(ctx->line.buf, command, ctx->line.size);

  shell_parse(ctx);

  int8_t result = SHELL_FAIL;

  ERROR_CHECK(shell_exec(ctx, &result),
    return SHELL_FAIL);

  return result;
}
