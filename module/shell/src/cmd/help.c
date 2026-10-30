#include <shell.h>
#include <string.h>

#if WITH_SHELL_HELP
static int8_t sh_cmd_help(shell_t * sh, uint8_t argc, char ** argv) {
  if (argc > 0) {
#define PRED(__x) !strcmp(__x->name, argv[0])
    const shell_cmd_t * cmd = LDS_GET_BY_PRED(shell_cmd, PRED);

    if (!cmd) {
      tty_printf(sh->tty, "Undefined command '%s'\r\n", argv[0]);
      return 1;
    }

    tty_printf(sh->tty, "%s - %s\r\n", cmd->name, cmd->short_help);
    tty_puts(sh->tty, cmd->help);
    tty_puts(sh->tty, "\r\n");

    return 0;
  }

  LDS_FOR_EACH(shell_cmd, cmd) {
    tty_printf(sh->tty, "%s - %s\r\n", cmd->name, cmd->short_help);
  }

  return 0;
}

SHELL_CMD_DECLARE(help, sh_cmd_help, "Help",
  "Show help for commands\r\n"
  "Usage:\r\n"
  " help - show every command with short help\r\n"
  " help <cmd> - show full help for a specific command"
);
#endif
