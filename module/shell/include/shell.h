#pragma once

#include <tty.h>
#include <shell/history.h>

/**
 * @brief Signals to the caller that an internal shell error happens while
 *        trying to execute a command
 */
#define SHELL_RES_INTERNAL_ERROR (-128)

/**
 * @brief Declare a shell command (automatically puts it into an LDS)
 *
 * Example:
 * @code{.c}
    int8_t sh_test(shell_t * sh, uint8_t argc, char ** argv) {
      return 0;
    }
    SHELL_CMD_DECLARE(test, sh_test, "test", "Test of SHELL_CMD_DECLARE");
 * @endcode
 */
#define SHELL_CMD_DECLARE(__name, __handler, __short_help, __help) \
  LDS_APPEND_NAMED(shell_cmd, UTIL_CAT(__name, _sh_cmd), {                    \
    .name = UTIL_STRINGIFY(__name),                                           \
    .handler = __handler,                                                     \
    UTIL_IF_1P(WITH_SHELL_HELP, (.short_help = __short_help,), ())            \
    UTIL_IF_1P(WITH_SHELL_HELP, (.help = __help,), ())                        \
  })

/**
 * @brief Actually creates an entry in a sub-cmd array
 *
 * @internal
 */
#define SHELL_SUBCMD_IMPL(__name, __handler, __help) \
  {                                                                           \
    .name = UTIL_STRINGIFY(__name),                                           \
    .handler = __handler,                                                     \
    UTIL_IF_1P(WITH_SHELL_HELP, (.help = __help,), ())                        \
  },

/**
 * @brief Used by SHELL_SUBCMD to unpack a sub-cmd tuple into usable named
 *        macro arguments
 *
 * @internal
 */
#define SHELL_SUBCMD_TRAMPOLINE(...) SHELL_SUBCMD_IMPL(__VA_ARGS__)

/**
 * @brief Used by SHELL_SUBCMD_DECLARE by calling it on each sub-cmd tuple
 *        to transform it into an init-expression for a shell_subcmd_t
 *
 * @internal
 */
#define SHELL_SUBCMD(...) SHELL_SUBCMD_TRAMPOLINE(UTIL_EXPAND __VA_ARGS__)

/**
 * @brief Declares a sub-command array for use with `shell_subcmd_execute`
 *
 * Example:
 * @code{.c}
    int8_t sh_test_help(shell_t * sh, uint8_t argc, char ** argv) {
      tty_printf(sh->tty, "Help for test command\n");
      return 0;
    }

    int8_t sh_test_arg(shell_t * sh, uint8_t argc, char ** argv) {
      for (uint8_t i = 0; i < argc; ++i) {
        tty_printf(sh->tty, "%s\n", argv[i]);
      }
      return 0;
    }

    SHELL_SUBCMD_DECLARE(
      test_subcmd,
      (help, sh_test_help, "Run help"),
      (arg,  sh_test_arg,  "Arg test")
    );

    int8_t sh_test(shell_t * sh, uint8_t argc, char ** argv) {
      int8_t result = 0;
      return shell_subcmd_execute(sh, &result, test_subcmd, argc, argv) == E_OK ? result : 1;
    }

    SHELL_CMD_DECLARE(test, sh_test, "subcmd test", "Test of SUBCMD mechanism");
 * @endcode
 */
#define SHELL_SUBCMD_DECLARE(__name, ...) \
  static const shell_subcmd_t __name[] = {\
    UTIL_FOR_EACH(SHELL_SUBCMD, __VA_ARGS__) \
    {NULL, NULL, UTIL_IF_1(WITH_SHELL_HELP, NULL)} \
  };

typedef struct shell shell_t;

typedef int8_t (*shell_cmd_handler_t)(shell_t * sh, uint8_t argc, char ** argv);

/**
 * @brief Represents a shell command descriptor
 */
typedef struct shell_cmd {
  const char *        name;
  shell_cmd_handler_t handler;
#if WITH_SHELL_HELP
  const char *        short_help;
  const char *        help;
#endif
} shell_cmd_t;

/**
 * @brief Shell command descriptors live in an LDS (as they're static data)
 */
LDS_DECLARE(shell_cmd, shell_cmd_t);

/**
 * @brief Represents a shell sub-command descriptor
 */
typedef struct shell_subcmd {
  const char *        name;
  shell_cmd_handler_t handler;
#if WITH_SHELL_HELP
  const char *        help;
#endif
} shell_subcmd_t;

/**
 * @brief Shell Flags
 */
typedef enum shell_flags {
  SHELL_FLAG_NONE = 0,
  SHELL_FLAG_SHOW_PROMPT = (1 << 0),
  SHELL_FLAG_ECHO_RESULT = (1 << 1),
} shell_flags_t;

/**
 * @brief Shell context
 */
typedef struct shell {
  /// Bound TTY
  tty_t * tty;

  /// Shell flags
  shell_flags_t flags;

  // TODO: user void* ctx ???
  // TODO: env

  /// Buffer for current input line
  char buf[SHELL_MAX_LINE_SIZE];

  /// Buffer for split argv (data lives in `buf`)
  struct {
    char *  buf[SHELL_MAX_TOKENS];
    uint8_t size;
  } args;

#if WITH_SHELL_HISTORY
  shell_history_t history;
#endif

} shell_t;

/**
 * @brief Initialize the shell, bind `tty` to it
 */
error_t shell_init(shell_t * sh, tty_t * tty);

/**
 * @brief Process shell input
 * @note Must be called in a loop
 * @note Supports a non-blocking mode, if TTY is in a non-blocking mode
 */
error_t shell_process(shell_t * sh);

/**
 * @brief Execute a `command`, putting it's result into `result`
 */
error_t shell_execute(shell_t * sh, int8_t * result, const char * command);

/**
 * @brief Execute a multiline (newline or semicolon separated) command
 */
error_t shell_execute_multiline(shell_t * sh, const char * command);

/**
 * @brief Helper for executing subcommands defined by `SHELL_SUBCMD_DECLARE`
 */
error_t shell_subcmd_execute(shell_t * sh, int8_t * result, const shell_subcmd_t * subcmd, uint8_t argc, char ** argv);
