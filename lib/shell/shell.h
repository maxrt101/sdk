/** ========================================================================= *
 *
 * @file shell.h
 * @date 19-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Shell library. Implements command line environment. Uses TTY
 *        abstraction for IO. Supports environment variables, custom commands,
 *        line parsing into tokens, backspace, arrow keys, history &
 *        environment variables.
 *
 * @note For environment variables to work define USE_SHELL_ENV in CMake
 * @note For history to work define USE_SHELL_HISTORY in CMake
 *
 * To control history size use SHELL_HISTORY_BUFFER_SIZE
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
#include <stdio.h>

/* Defines ================================================================== */
/**
 * Max size of one line read from input
 */
#ifndef SHELL_MAX_LINE_SIZE
#define SHELL_MAX_LINE_SIZE         32
#endif

/**
 * Max number of tokens
 */
#ifndef SHELL_MAX_TOKENS
#define SHELL_MAX_TOKENS            16
#endif

/**
 * Max size of variable name
 */
#ifndef SHELL_VAR_NAME_SIZE
#define SHELL_VAR_NAME_SIZE         8
#endif

/**
 * Max size of variable value
 */
#ifndef SHELL_VAR_VALUE_SIZE
#define SHELL_VAR_VALUE_SIZE        16
#endif

/**
 * Max variable count
 */
#ifndef SHELL_VAR_BUFFER_SIZE
#define SHELL_VAR_BUFFER_SIZE       4
#endif

/**
 * History buffer size
 *
 * @note Will use SHELL_MAX_LINE_SIZE * SHELL_HISTORY_BUFFER_SIZE bytes
 */
#ifndef SHELL_HISTORY_BUFFER_SIZE
#define SHELL_HISTORY_BUFFER_SIZE   4
#endif

/**
 * Successful result
 */
#define SHELL_OK    0

/**
 * Operation failed
 */
#define SHELL_FAIL  1

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
    uint8_t      size;
  } args;

#if USE_SHELL_HISTORY
  struct {
    /** History buffer */
    struct {
      char line[SHELL_MAX_LINE_SIZE];
    } buffer[SHELL_HISTORY_BUFFER_SIZE];

    /** Index into history. Used by UP/DOWN arrow key handlers */
    ssize_t index;

    /** Points to last command */
    ssize_t head;
  } history;
#endif

#if USE_SHELL_ENV
  /** Environment. Holds limited amount of variables */
  // TODO: Use table_t
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
 * @param sh Shell module context
 * @param file File used for IO
 * @param handler_ctx Context for commands handlers (user-defined)
 */
error_t shell_init(shell_t * sh, vfs_file_t * file, void * handler_ctx);

/**
 * Starts shell
 *
 * @param sh Shell module context
 */
error_t shell_start(shell_t * sh);

/**
 * Stops shell
 *
 * @param sh Shell module context
 */
error_t shell_stop(shell_t * sh);

/**
 * Return true if shell is running, false otherwise
 *
 * @param sh Shell module context
 */
bool shell_is_running(shell_t * sh);

/**
 * Runs shell command processor
 *
 * @param sh Shell module context
 */
error_t shell_process(shell_t * sh);

/**
 * Parses and runs the command
 *
 * @param sh Shell module context
 * @param command Command
 */
int8_t shell_execute(shell_t * sh, const char * command);

#if USE_SHELL_HISTORY
/**
 * Reset shell history
 *
 * @param sh Shell context
 */
error_t shell_history_clear(shell_t * sh);

/**
 * Reset shell history index
 *
 * @note Should be called on each new command to reset history index
 *
 * @param sh Shell context
 */
error_t shell_history_reset(shell_t * sh);

/**
 * Appends a line (command) to history
 *
 * @note Will reject empty lines
 *
 * @param sh Shell context
 * @param line Line (command) to append
 */
error_t shell_history_append(shell_t * sh, tty_line_t * line);

/**
 * Get history line from buffer at current history index
 *
 * @param sh Shell context
 * @param line Line where to put a command from history
 *
 * @returns E_OK - if
 */
error_t shell_history_get(shell_t * sh, tty_line_t * line);

/**
 * Advance history buffer index
 *
 * @param sh Shell context
 * @returns E_OK - if can advance.
 */
error_t shell_history_advance(shell_t * sh);

/**
 * Retard history buffer index
 *
 * @param sh Shell context
 * @returns E_OK - if can retard.
 */
error_t shell_history_retard(shell_t * sh);

/**
 * Prepares shell_history_process_ansi_csi for parsing escape sequences
 *
 * Sets hidden global variable with shell context for
 * shell_history_process_ansi_csi to use
 *
 * @param sh Shell context
 */
void shell_history_prepare_ansi_processor(shell_t * sh);

/**
 * Custom handler for ANSI CSI sequences that handles `ESC [ A` & `ESC [ B`
 *
 * @note Before calling tty_read_line* should call shell_history_prepare_ansi_processor
 *
 * @param tty TTY Context
 * @param line TTY Line Context
 * @param c Received Character
 */
error_t shell_history_process_ansi_csi(tty_t * tty, tty_line_t * line, char c);
#endif

#if USE_SHELL_ENV
/**
 * Find value of a variable and place it in *value
 *
 * @param[in] sh Shell module context
 * @param[in] name Variable name
 * @param[out] value Variable value
 */
error_t shell_env_find(shell_t * sh, const char * name, char ** value);

/**
 * Set variable value (create new if not present)
 *
 * @param[in] sh Shell module context
 * @param[in] name Variable name
 * @param[in] value Variable value
 */
error_t shell_env_set(shell_t * sh, const char * name, const char * value);

/**
 * Remove variable
 *
 * @param[in] sh Shell module context
 * @param[in] name Variable name
 */
error_t shell_env_unset(shell_t * sh, const char * name);

/**
 * Parse argument - if begins with $ - return variable value, otherwise return arg
 *
 * @param[in] sh Shell module context
 * @param[in] arg Value to parse
 */
const char * shell_arg_parse(shell_t * sh, const char * arg);
#endif

#ifdef __cplusplus
}
#endif