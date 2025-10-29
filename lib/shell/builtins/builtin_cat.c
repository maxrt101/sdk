/** ========================================================================= *
*
 * @file builtin_cat.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'cat' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "time/sleep.h"
#include "error/assertion.h"
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
int8_t builtin_cat(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc != 2) {
    log_error("Usage: cat FILE");
    return SHELL_FAIL;
  }

  vfs_file_t * file = vfs_open(&vfs, argv[1]);

  if (!file) {
    log_error("Can't open file '%s'", argv[1]);
    return SHELL_FAIL;
  }

  size_t size = vfs_get_file_size(file);

  for (size_t i = 0; i < size; ++i) {
    uint8_t ch;
    vfs_read(file, &ch, 1, VFS_READ_FLAG_NONE);
    log_printf("%c", ch);
  }
  log_printf("\r\n");

  vfs_close(file);

  return SHELL_OK;
}
#endif
