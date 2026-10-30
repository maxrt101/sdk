#include <shell.h>
#include <string.h>

#if WITH_SHELL_HISTORY
static int8_t sh_cmd_history(shell_t * sh, uint8_t argc, char ** argv) {
  if (argc > 0 && !strcmp(argv[0], "clear")) {
    shell_history_clear(sh);
    return 0;
  }

  ASSERT_RETURN(sh->history.head != -1, 0);

  ssize_t index = (sh->history.head + 1) % SHELL_HISTORY_BUFFER_SIZE;

  for (int8_t i = 0; i < SHELL_HISTORY_BUFFER_SIZE; ++i) {
    char buf[SHELL_MAX_LINE_SIZE];

    if (shell_history_get_at(sh, index, buf, SHELL_MAX_LINE_SIZE) == E_OK) {
      tty_printf(sh->tty, "%s\r\n", buf);
    }

    if (index == sh->history.head) {
      break;
    }

    index = (index + 1) % SHELL_HISTORY_BUFFER_SIZE;
  }

  return 0;
}

SHELL_CMD_DECLARE(history, sh_cmd_history, "History",
  "Show history\r\n"
  "Usage:\r\n"
  " history - show command history\r\n"
  " history clear - clear history"
);
#endif
