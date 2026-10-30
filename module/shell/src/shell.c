#include <shell.h>
#include <tty/ansi.h>
#include <string.h>

LDS_DEFINE(shell_cmd);

void shell_parse(shell_t * sh);

__STATIC_INLINE void shell_reset_buffers(shell_t * sh) {
  memset(sh->buf, 0, sizeof(sh->buf));

  memset(sh->args.buf, 0, sizeof(sh->args.buf));
  sh->args.size = 0;
}

__STATIC_INLINE error_t shell_exec(shell_t * sh, int8_t * result) {
  ASSERT_RETURN(sh && result, E_NULL);

  LDS_FOR_EACH(shell_cmd, cmd) {
    if (!strcmp(sh->args.buf[0], cmd->name)) {
      *result = cmd->handler(sh, sh->args.size - 1, &sh->args.buf[1]);

      if (sh->flags & SHELL_FLAG_ECHO_RESULT) {
        tty_printf(sh->tty, "=%d\r\n", *result);
      }

      return E_OK;
    }
  }

  tty_printf(sh->tty, "Command '%s' not found\r\n", sh->args.buf[0]);

  *result = SHELL_RES_INTERNAL_ERROR;

  return E_NOTFOUND;
}

__STATIC_INLINE void shell_exec_multiline(shell_t * sh, const char ** start, const char ** end) {
  size_t size = *end - *start;
  memcpy(sh->buf, *start, size);
  sh->buf[size] = '\0';

  shell_parse(sh);

  // TODO: Accumulate result and pass out of the function?
  __UNUSED int8_t result = 0;

  shell_exec(sh, &result);

  *start = *end + 1;
}

void shell_print_line(shell_t * sh, bool force) {
  bool write_detected = sh->tty->flags & TTY_FLAG_WRITE_DETECTION;

  if (!force && !write_detected) {
    return;
  }

  tty_puts(sh->tty, ANSI_ERASE_FROM_CURSOR_TO_LINE_START "\r");

  if (sh->flags & SHELL_FLAG_SHOW_PROMPT) {
    tty_puts(sh->tty, SHELL_PROMPT);
  }

  if (sh->tty->input.size) {
    tty_write(sh->tty, sh->tty->input.buf, sh->tty->input.size);
  }

  if (sh->flags & SHELL_FLAG_SHOW_PROMPT) {
    tty_printf(sh->tty, "\r" ANSI_CURSOR_MOVE_RIGHT_FMT, (int)(sh->tty->input.cursor + sizeof(SHELL_PROMPT) - 1));
  } else {
    tty_printf(sh->tty, "\r" ANSI_CURSOR_MOVE_RIGHT_FMT, sh->tty->input.cursor); // TODO: -1 ?
  }

  sh->tty->flags &= ~TTY_FLAG_WRITE_DETECTION;
}

error_t shell_init(shell_t * sh, tty_t * tty) {
  ASSERT_RETURN(sh && tty, E_NULL);

  memset(sh, 0, sizeof(shell_t));

  sh->tty = tty;
  sh->flags = SHELL_FLAG_SHOW_PROMPT;

  // Trigger prompt re-render
  sh->tty->flags |= TTY_FLAG_WRITE_DETECTION;

#if WITH_SHELL_HISTORY
  shell_history_init(sh);
#endif

  return E_OK;
}

error_t shell_process(shell_t * sh) {
  ASSERT_RETURN(sh, E_NULL);

  shell_print_line(sh, false);

  // When full line is received, this function returns E_OK, E_AGAIN otherwise
  ERROR_CHECK_RETURN(tty_readline(sh->tty, sh->buf, sizeof(sh->buf)));

  // If execution reached here, tty_read_line_async returned E_OK, and line
  // is present in the buffer

#if WITH_SHELL_HISTORY
  shell_history_append(sh, sh->buf);
  shell_history_reset_index(sh);
#endif

  shell_parse(sh);

  if (!sh->args.size || !strlen(sh->args.buf[0])) {
    return E_EMPTY;
  }

  int8_t result = SHELL_RES_INTERNAL_ERROR;

  shell_exec(sh, &result);

  shell_reset_buffers(sh);

  return result == 0 ? E_OK : E_FAILED;
}

error_t shell_execute(shell_t * sh, int8_t * result, const char * command) {
  ASSERT_RETURN(sh && command, E_NULL);

  strcpy(sh->buf, command);

  shell_parse(sh);

  *result = SHELL_RES_INTERNAL_ERROR;

  return shell_exec(sh, result);
}

error_t shell_execute_multiline(shell_t * sh, const char * command) {
  ASSERT_RETURN(sh && command, E_NULL);

  const char * start = command;
  const char * end = command;

  while (*end) {
    if (*end == ';' || *end == '\n') {
      shell_exec_multiline(sh, &start, &end);
    }

    end++;
  }

  if (start != end + 1) {
    shell_exec_multiline(sh, &start, &end);
  }

  return E_OK;
}

error_t shell_subcmd_execute(shell_t * sh, int8_t * result, const shell_subcmd_t * subcmd, uint8_t argc, char ** argv) {
  ASSERT_RETURN(sh && subcmd, E_NULL);
  ASSERT_RETURN(argc > 0, E_EMPTY);

  for (size_t i = 0; subcmd[i].name; ++i) {
    if (!strcmp(subcmd[i].name, argv[0])) {
      *result = subcmd[i].handler(sh, argc - 1, &argv[1]);
      return E_OK;
    }
  }

  return E_NOTFOUND;
}
