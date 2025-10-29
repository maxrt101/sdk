/** ========================================================================= *
*
 * @file builtin_mkdir.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'mkdir' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "shell/shell_util.h"
#include "time/sleep.h"
#include "log/log.h"
#include "vfs/vfs.h"

/* Defines ================================================================== */
#define LOG_TAG SHELL

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
#if USE_GLOBAL_VFS
int8_t builtin_mkdir(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc != 2) {
    log_error("Usage: mkdir PATH");
    return SHELL_FAIL;
  }

  SHELL_ERR_REPORT_RETURN(vfs_mkdir(&vfs, argv[1]), "vfs_mkdir");

  return SHELL_OK;
}
#endif
