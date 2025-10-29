/** ========================================================================= *
*
 * @file builtin_ls.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'ls' builtin cli command implementation
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
int8_t builtin_ls(shell_t * sh, uint8_t argc, const char ** argv) {
  const char * path = "/";
  bool long_format = false;

  for (uint8_t i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-l")) {
      long_format = true;
    } else {
      path = argv[i];
    }
  }

  vfs_file_t * folder = vfs_open(&vfs, path);

  SHELL_ASSERT_REPORT_RETURN(folder, "Failed to open folder");

  for (size_t i = 0; i < folder->folder.children->capacity; ++i) {
    if (!folder->folder.children->nodes[i].used) {
      continue;
    }

    vfs_file_t * iter = (vfs_file_t *) folder->folder.children->nodes[i].value;

    if (long_format) {
      if (iter->head.type == VFS_FILE) {
        log_printf("'%s' %s flags=0x%x cap=%u size=%u ofs=%u\r\n",
          vfs_get_file_name(iter),
          vfs_node_type_to_string(iter->head.type),
          iter->head.flags,
          iter->file.data.capacity,
          iter->file.data.size,
          iter->file.data.offset);
      } else {
        log_printf("'%s' %s flags=0x%x\r\n",
          vfs_get_file_name(iter),
          vfs_node_type_to_string(iter->head.type),
          iter->head.flags);
      }
    } else {
      log_printf("%s\r\n", vfs_get_file_name(iter));
    }
  }

  return SHELL_OK;
}
#endif
