/** ========================================================================= *
 *
 * @file shell_builtins.h
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Builtin command handlers forward declarations
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "shell/shell.h"
#include <string.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
int8_t builtin_abort(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_clear(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_echo(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_exec(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_exit(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_help(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_if(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_log(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_mem(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_pwr(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_repeat(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_reset(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_sh(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_sleep(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_task(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_time(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_tty(shell_t * sh, uint8_t argc, const char ** argv);

#if USE_SHELL_HISTORY
int8_t builtin_history(shell_t * sh, uint8_t argc, const char ** argv);
#endif

#if USE_GLOBAL_VFS
int8_t builtin_cat(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_hexdump(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_ls(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_mkdir(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_rm(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_touch(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_write(shell_t * sh, uint8_t argc, const char ** argv);
#endif

#if USE_SHELL_ENV
int8_t builtin_check(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_expr(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_set(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_unset(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_env(shell_t * sh, uint8_t argc, const char ** argv);
int8_t builtin_input(shell_t * sh, uint8_t argc, const char ** argv);
#endif

#ifdef __cplusplus
}
#endif