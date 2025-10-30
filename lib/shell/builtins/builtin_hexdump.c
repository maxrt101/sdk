/** ========================================================================= *
*
 * @file builtin_hexdump.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'hexdump' builtin cli command implementation
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

#define SH_HEXDUMP_LINE 16

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
#if USE_GLOBAL_VFS
int8_t builtin_hexdump(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc != 2) {
    log_error("Usage: hexdump FILE");
    return SHELL_FAIL;
  }

  vfs_file_t * file = vfs_open(&vfs, argv[1]);

  if (!file) {
    log_error("Can't open file '%s'", argv[1]);
    return SHELL_FAIL;
  }

  size_t size = vfs_get_file_size(file);

  // TODO: Propper formatting (add address & colors + alterating bold/default fot data)

  for (size_t i = 0; i < size; ++i) {
    uint8_t byte;
    vfs_read(file, &byte, 1, VFS_READ_FLAG_NONE);
    log_printf("%02x ", byte);

    if ((i+1) % SH_HEXDUMP_LINE == 0) {
      log_printf("\r\n");
    }
  }

  log_printf("\r\n");

  vfs_close(file);

  return SHELL_OK;
}
#endif
