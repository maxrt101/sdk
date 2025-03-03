/** ========================================================================= *
 *
 * @file fs.c
 * @date 28-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief FS API Implementation using VFS
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include <string.h>
#include "os/fs/fs.h"
#include "error/assertion.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
static struct {
  vfs_t root;
} os_fs_ctx;

/* Private functions ======================================================== */
/* Shared functions ========================================================= */
error_t os_mount(const char * path, fs_t * fs) {
  ASSERT_RETURN(path && fs, E_NULL);
  strcpy(fs->mount_point, path);
  return vfs_mount(&os_fs_ctx.root, fs);
}

error_t os_unmount(const char * path) {
  ASSERT_RETURN(path, E_NULL);

  vfs_t * vfs = &os_fs_ctx.root;

  while (vfs) {
    if (!memcmp(vfs->mount_point, path, strlen(vfs->mount_point))) {
      break;
    }
    vfs = vfs->impl.next;
  }

  ASSERT_RETURN(vfs, E_NOTFOUND);

  return vfs_unmount(&os_fs_ctx.root, vfs);
}

error_t os_create(const char * path, os_file_t * file) {
  ASSERT_RETURN(path && file, E_NULL);
  strcpy(file->name, path);
  return vfs_create(&os_fs_ctx.root, file);
}

error_t os_remove(const char * path) {
  ASSERT_RETURN(path, E_NULL);
  os_file_t * file = vfs_open(&os_fs_ctx.root, path);
  return vfs_remove(&os_fs_ctx.root, file);
}

error_t os_rename(const char * old_name, const char * new_name) {
  ASSERT_RETURN(old_name && new_name, E_NULL);
  return vfs_rename(&os_fs_ctx.root, old_name, new_name);
}

os_file_t * os_open(const char * path) {
  ASSERT_RETURN(path, NULL);
  return vfs_open(&os_fs_ctx.root, path);
}

error_t os_close(os_file_t * file) {
  ASSERT_RETURN(file, E_NULL);
  return vfs_close(&os_fs_ctx.root, file);
}

error_t os_write(os_file_t * file, const uint8_t * buffer, size_t size) {
  ASSERT_RETURN(file && buffer, E_NULL);
  return vfs_write(file, buffer, size);
}

error_t os_read(os_file_t * file, uint8_t * buffer, size_t size) {
  ASSERT_RETURN(file && buffer, E_NULL);
  return vfs_read(file, buffer, size);
}

error_t os_seek(os_file_t * file, size_t offset) {
  return vfs_seek(file, offset);
}

size_t os_tell(os_file_t * file) {
  return vfs_tell(file);
}

error_t os_ioctl(os_file_t * file, int cmd, ...) {
  ASSERT_RETURN(file, E_NULL);

  va_list args;

  va_start(args, cmd);

  error_t err = vfs_ioctl(file, cmd, args);

  va_end(args);

  return err;
}
