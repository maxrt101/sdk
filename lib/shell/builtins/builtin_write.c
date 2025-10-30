/** ========================================================================= *
*
 * @file builtin_write.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'write' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "time/sleep.h"
#include "error/assertion.h"
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
int8_t builtin_write(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc < 3) {
    log_error("Usage: write FILE TEXT");
    return SHELL_FAIL;
  }

  vfs_file_t * file = vfs_open(&vfs, argv[1]);

  if (!file) {
    log_error("Can't open file '%s'", argv[1]);
    return SHELL_FAIL;
  }

  size_t size = vfs_get_file_size(file);
  vfs_seek(file, size);

  for (uint8_t i = 2; i < argc; ++i) {
    vfs_write(file, argv[i], strlen(argv[i]));
  }

  vfs_close(file);

  return SHELL_OK;
}
#endif
