/** ========================================================================= *
*
 * @file builtin_touch.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'touch' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "shell/shell_util.h"
#include "time/sleep.h"
#include "log/log.h"
#include "vfs/vfs.h"

/* Defines ================================================================== */
#define LOG_TAG shell

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
#if USE_GLOBAL_VFS
int8_t builtin_touch(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc != 3) {
    log_error("Usage: touch PATH SIZE");
    return SHELL_FAIL;
  }

  SHELL_ERR_REPORT_RETURN(
    vfs_create_file(&vfs, argv[1], &(vfs_file_data_t){.buffer = NULL, .capacity = shell_parse_int(argv[2])}),
    "vfs_crate_file");

  return SHELL_OK;
}
#endif
