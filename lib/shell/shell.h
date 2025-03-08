/** ========================================================================= *
 *
 * @file shell.h
 * @date 19-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Shell library. Implements command line environment. Uses UART for IO.
 *        Supports environment variables, custom commands, line parsing into
 *        tokens, backspace, etc.
 *
 * TODO: History buffer, handle special chars better (backspace, arrows, ...)
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/compiler.h"
#include "util/util.h"
#include "vfs/vfs.h"
#include "tty/tty.h"
#include <stdbool.h>
#include <stdlib.h>

/* Defines ================================================================== */
/**
 * Max size of one line read from input
 */
#ifndef SHELL_MAX_LINE_SIZE
#define SHELL_MAX_LINE_SIZE     64
#endif

/**
 * Max number of tokens
 */
#ifndef SHELL_MAX_TOKENS
#define SHELL_MAX_TOKENS        16
#endif

/**
 * Max size of variable name
 */
#ifndef SHELL_VAR_NAME_SIZE
#define SHELL_VAR_NAME_SIZE     8
#endif

/**
 * Max size of variable value
 */
#ifndef SHELL_VAR_VALUE_SIZE
#define SHELL_VAR_VALUE_SIZE    16
#endif

/**
 * Max variable count
 */
#ifndef SHELL_VAR_BUFFER_SIZE
#define SHELL_VAR_BUFFER_SIZE   8
#endif

/**
 * Successful result
 */
#define SHELL_OK                0

/**
 * Operation failed
 */
#define SHELL_FAIL              1

/**
 * Internal macro
 * Defines shell command type (for iterators, section start/end markers, etc)
 */
#define __SH_CMD_TYPE const shell_command_t *

/* Macros =================================================================== */
/**
 * Declares shell command in shell linker section
 */
#define SHELL_DECLARE_COMMAND(__name, __fn, __help)                           \
  __USED const shell_command_t UTIL_CAT(__name, _shell_command)               \
    __SECTION(sh_cmd) = {#__name, __fn, __help}

/**
 * Iterates through all shell commands
 *
 * Example:
 * @code{.c}
 *   SHELL_ITER_COMMANDS(cmd) {
 *     log_info("%s", cmd->name);
 *   }
 * @endcode
 *
 * @param __iter name of variable that will be the pointer to command
 */
#define SHELL_ITER_COMMANDS(__iter)                                           \
  extern __SH_CMD_TYPE __start_sh_cmd;                                        \
  extern __SH_CMD_TYPE __end_sh_cmd;                                          \
  for (__SH_CMD_TYPE __iter = (__SH_CMD_TYPE) &__start_sh_cmd;                \
      __iter != (__SH_CMD_TYPE) &__end_sh_cmd;                                \
      __iter++)

/* Enums ==================================================================== */
/**
 * Shell state
 */
typedef enum {
  SHELL_STATE_IDLE = 0,
  SHELL_STATE_RUNNING,
} shell_state_t;

/**
 * Shell flags
 */
typedef enum {
  SHELL_FLAG_SHOWPROMPT = (1 << 0),  /** Print prompt each line */
  SHELL_FLAG_ECHO_RES   = (1 << 1),  /** Echo command result */
} shell_flag_t;

/* Types ==================================================================== */
/**
 * Forward declaration of shell_t
 */
typedef struct shell_s shell_t;

/**
 * Shell command handler
 */
typedef int8_t (*shell_command_handler_t)(shell_t *, uint8_t, const char **);

/**
 * Shell command
 */
typedef struct {
  const char * name;
  shell_command_handler_t handler;
  const char * help;
} shell_command_t;

/**
 * Shell context
 */
typedef struct shell_s {
  /** Shell state */
  shell_state_t state;

  /** TTY for IO */
  tty_t tty;

  /** Input line */
  tty_line_t line;

  /** Shel flags */
  uint8_t flags;

  /** Internal flags */
  __PACKED_STRUCT {
    bool is_new_line : 1;
  } internal_flags;

  /** Holds user-defined commands context */
  struct {
    void * ctx;
  } commands;

  /** Argument string pointer array (argc, argv) */
  struct {
    const char * buf[SHELL_MAX_TOKENS];
    uint8_t size;
  } ptr;

#if USE_SHELL_ENV
  /** Environment. Holds limited amount of variables */
  struct {
    char name[SHELL_VAR_NAME_SIZE];
    char value[SHELL_VAR_VALUE_SIZE];
    bool used;
  } vars[SHELL_VAR_BUFFER_SIZE];
#endif
} shell_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes shell
 *
 * @param ctx Shell module context
 * @param file File used for IO
 * @param handler_ctx Context for commands handlers (user-defined)
 */
error_t shell_init(
    shell_t * ctx, vfs_file_t * file, void * handler_ctx);

/**
 * Starts shell
 *
 * @param ctx Shell module context
 */
error_t shell_start(shell_t * ctx);

/**
 * Stops shell
 *
 * @param ctx Shell module context
 */
error_t shell_stop(shell_t * ctx);

/**
 * Return true if shell is running, false otherwise
 *
 * @param ctx Shell module context
 */
bool shell_is_running(shell_t * ctx);

/**
 * Runs shell command processor
 *
 * @param ctx Shell module context
 */
error_t shell_process(shell_t * ctx);

/**
 * Parses and runs the command
 *
 * @param ctx Shell module context
 * @param command Command
 */
int8_t shell_execute(shell_t * ctx, const char * command);

#if USE_SHELL_ENV
/**
 * Find value of a variable and place it in *value
 *
 * @param[in] ctx Shell module context
 * @param[in] name Variable name
 * @param[out] value Variable value
 */
error_t shell_env_find(shell_t * ctx, const char * name, char ** value);

/**
 * Set variable value (create new if not present)
 *
 * @param[in] ctx Shell module context
 * @param[in] name Variable name
 * @param[in] value Variable value
 */
error_t shell_env_set(shell_t * ctx, const char * name, const char * value);

/**
 * Remove variable
 *
 * @param[in] ctx Shell module context
 * @param[in] name Variable name
 */
error_t shell_env_unset(shell_t * ctx, const char * name);

/**
 * Parse argument - if begins with $ - return variable value, otherwise return arg
 *
 * @param[in] ctx Shell module context
 * @param[in] arg Value to parse
 */
const char * shell_arg_parse(shell_t * ctx, const char * arg);
#endif

#ifdef __cplusplus
}
#endif