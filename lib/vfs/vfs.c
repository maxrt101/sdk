/** ========================================================================= *
 *
 * @file vfs.c
 * @date 28-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief VFS Implementation
 *        Uses very simple algorithm, which will be slow on large amount of
 *        files, but it was intended to be used on small amount of block
 *        devices, mainly to keep track of IO (uart, etc.) and provide
 *        generalized API for read/write peripherals
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "vfs/vfs.h"
#include "error/assertion.h"
#include <string.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
static vfs_t * vfs_root = NULL;

/* Private functions ======================================================== */
/* Shared functions ========================================================= */
vfs_t * vfs_find_by_file_prefix(vfs_t * root, const char * filename) {
  vfs_t * vfs = root;

  while (vfs) {
    if (!memcmp(vfs->mount_point, filename, strlen(vfs->mount_point))) {
      break;
    }
    vfs = vfs->impl.next;
  }

  return vfs;
}

vfs_file_t * vfs_find_file(vfs_t * root, const char * filename) {
  vfs_t * vfs = vfs_find_by_file_prefix(root, filename);

  if (!vfs) {
    return NULL;
  }

  vfs_file_t * file_iter = vfs->files;

  while (file_iter) {
    if (!strcmp(file_iter->name, filename)) {
      return file_iter;
    }
    file_iter = file_iter->impl.next;
  }

  return NULL;
}

error_t vfs_init(vfs_t * vfs) {
  ASSERT_RETURN(vfs, E_NULL);

  memset(vfs, 0, sizeof(*vfs));

  return E_OK;
}

error_t vfs_set_root(vfs_t * vfs) {
  vfs_root = vfs;
  return E_OK;
}

error_t vfs_mount(const char * mountpoint, vfs_t * vfs) {
  ASSERT_RETURN(vfs_root && vfs && mountpoint, E_NULL);
  ASSERT_RETURN(vfs_root != vfs, E_INVAL);

  strcpy(vfs->mount_point, mountpoint);

  vfs_t * iter = vfs_root;
  while (iter->impl.next) {
    iter = iter->impl.next;
  }
  iter->impl.next = vfs;

  return E_OK;
}

error_t vfs_unmount(vfs_t * vfs) {
  ASSERT_RETURN(vfs_root && vfs, E_NULL);
  ASSERT_RETURN(vfs_root != vfs, E_INVAL);

  vfs_t * iter = vfs_root;
  while (iter->impl.next != vfs) {
    if (!iter->impl.next) {
      return E_NOTFOUND;
    }
    iter = iter->impl.next;
  }
  iter->impl.next = iter->impl.next->impl.next;

  return E_OK;
}

error_t vfs_create(const char * path, vfs_file_t * file) {
  ASSERT_RETURN(vfs_root && file && path, E_NULL);

  strcpy(file->name, path);

  vfs_t * vfs = vfs_find_by_file_prefix(vfs_root, file->name);

  if (!vfs) {
    return E_INVAL;
  }

  if (!vfs->files) {
    vfs->files = file;
    file->impl.next = NULL;
    file->impl.vfs = vfs;
  }

  vfs_file_t * file_iter = vfs->files;

  while (file_iter->impl.next) {
    file_iter = file_iter->impl.next;
  }

  file_iter->impl.next = file;
  file->impl.vfs = vfs;

  return E_OK;
}

error_t vfs_remove(vfs_file_t * file) {
  ASSERT_RETURN(vfs_root && file, E_NULL);

  vfs_t * vfs = vfs_find_by_file_prefix(vfs_root, file->name);
  vfs_file_t * file_iter = vfs->files;

  if (vfs->files == file) {
    vfs->files = vfs->files->impl.next;
    return E_OK;
  }

  while (file_iter->impl.next) {
    if (file_iter->impl.next == file) {
      break;
    }
    file_iter = file_iter->impl.next;
  }

  file_iter->impl.next = file_iter->impl.next->impl.next;

  return E_OK;
}

error_t vfs_rename(const char * old_name, const char * new_name) {
  ASSERT_RETURN(vfs_root && old_name && new_name, E_NULL);

  vfs_file_t * file = vfs_find_file(vfs_root, old_name);

  if (file) {
    UTIL_STR_COPY(file->name, new_name, VFS_MAX_PATH);
    return E_OK;
  }

  return E_NOTFOUND;
}

vfs_file_t * vfs_open(const char * filename) {
  ASSERT_RETURN(vfs_root, NULL);

  vfs_file_t * file = vfs_find_file(vfs_root, filename);

  ASSERT_RETURN(file, NULL);

  switch (file->type) {
    case VFS_BLOCK:
      if (file->block.open) {
        ASSERT_RETURN(file->block.open(file->block.ctx, file) == E_OK, NULL);
      }
      return file;

    case VFS_SYMLINK:
      return vfs_open(file->symlink.name);

    case VFS_HARDLINK:
      return file->hardlink.file;

    case VFS_FILE:
    default:
      return file;
  }
}

error_t vfs_close(vfs_file_t * file) {
  return E_OK;
}

error_t vfs_write(vfs_file_t * file, const uint8_t * buffer, size_t size) {
  ASSERT_RETURN(file && buffer, E_NULL);

  switch (file->type) {
    case VFS_FILE: {
      size_t wr_size = UTIL_MIN(file->file.size - file->file.offset, size);
      void * res = memcpy(file->file.buffer + file->file.offset, buffer, wr_size);
      file->file.offset += wr_size;
      return res == buffer ? E_OK : E_FAILED;
    }

    case VFS_BLOCK: {
      ASSERT_RETURN(file->block.write, E_NOTIMPL);
      return file->block.write(file->block.ctx, file, buffer, size);
    }

    case VFS_SYMLINK:
    case VFS_HARDLINK:
    default:
      return E_NOTFOUND;
  }
}

error_t vfs_read(vfs_file_t * file, uint8_t * buffer, size_t size) {
  ASSERT_RETURN(file && buffer, E_NULL);

  switch (file->type) {
    case VFS_FILE: {
      size_t rd_size = UTIL_MIN(file->file.size - file->file.offset, size);
      void * res = memcpy(buffer, file->file.buffer, rd_size);
      file->file.offset += rd_size;
      return res == buffer ? E_OK : E_FAILED;
    }

    case VFS_BLOCK: {
      ASSERT_RETURN(file->block.read, E_NOTIMPL);
      return file->block.read(file->block.ctx, file, buffer, size);
    }

    case VFS_SYMLINK:
    case VFS_HARDLINK:
    default:
      return E_NOTIMPL;
  }
}

error_t vfs_seek(vfs_file_t * file, size_t offset) {
  ASSERT_RETURN(file, E_NULL);

  switch (file->type) {
    case VFS_BLOCK: {
      ASSERT_RETURN(file->block.seek, E_NOTIMPL);
      return file->block.seek(file->block.ctx, file, offset);
    }

    case VFS_FILE:
      file->file.offset = offset;
      return E_OK;

    case VFS_SYMLINK:
    case VFS_HARDLINK:
    default:
      return E_NOTIMPL;
  }
}

size_t vfs_tell(vfs_file_t * file) {
  ASSERT_RETURN(file, E_NULL);

  switch (file->type) {
    case VFS_BLOCK: {
      ASSERT_RETURN(file->block.seek, E_NOTIMPL);
      return file->block.tell(file->block.ctx, file);
    }

    case VFS_FILE:
      return file->file.offset;

    case VFS_SYMLINK:
    case VFS_HARDLINK:
    default:
      return 0;
  }
}

error_t vfs_ioctl(vfs_file_t * file, int cmd, ...) {
  ASSERT_RETURN(file, E_NULL);

  va_list args;

  va_start(args, cmd);
  error_t err = vfs_ioctl_va(file, cmd, args);
  va_end(args);

  return err;
}


error_t vfs_ioctl_va(vfs_file_t * file, int cmd, va_list args) {
  ASSERT_RETURN(file, E_NULL);

  switch (file->type) {
    case VFS_BLOCK: {
      ASSERT_RETURN(file->block.ioctl, E_NOTIMPL);
      return file->block.ioctl(file->block.ctx, file, cmd, args);
    }

    case VFS_FILE:
    case VFS_SYMLINK:
    case VFS_HARDLINK:
    default:
      return E_NOTIMPL;
  }
}
