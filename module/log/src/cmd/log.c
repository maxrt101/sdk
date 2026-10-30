#include <util.h>
#include <log.h>
#include <string.h>
#include <stdlib.h>

#if has_module(shell)

#include <shell.h>

static int8_t sh_cmd_log(shell_t * sh, uint8_t argc, char ** argv) {
  if (argc == 2) {
#define PRED(__x) !strcmp(__x->name, argv[0])
    const logger_t * logger = LDS_GET_BY_PRED(loggers, PRED);

    if (!logger) {
      tty_printf(sh->tty, "Undefined logger '%s'\r\n", argv[0]);
      return 1;
    }

    int8_t level = (int8_t)strtol(argv[1], NULL, 10);

    if (level < LOG_DISABLED || level >= LOG_MAX) {
      tty_printf(sh->tty, "Bad log level '%s'\r\n", argv[1]);
      return 1;
    }

    logger->rt->level = level;
  } else {
    LDS_FOR_EACH(loggers, logger) {
      tty_printf(sh->tty, "%s - %d\r\n", logger->name, logger->rt->level);
    }
  }

  return 0;
}

SHELL_CMD_DECLARE(log, sh_cmd_log, "Log control",
  "Set log level per-module\r\n"
  "Usage:\r\n"
  " log - show log level for each module\r\n"
  " log <module> <level> - set log level for module\r\n"
  "Log level can be between -1 to 4:\r\n"
  " -1 disabled\r\n"
  "  0 debug\r\n"
  "  1 info\r\n"
  "  2 warn\r\n"
  "  3 error\r\n"
  "  4 fatal"
);

#endif