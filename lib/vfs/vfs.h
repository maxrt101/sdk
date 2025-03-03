/** ========================================================================= *
 *
 * @file vfs.h
 * @date 28-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief VFS (VirtualFileSystem) Module
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

/* Defines ================================================================== */
#define VFS_MAX_PATH 32 /** VFS Maximum Path Size */

/* Macros =================================================================== */
/* Enums ==================================================================== */
/**
 * VFS File Type
 */
typedef enum {
  VFS_NONE = 0,
  VFS_FILE,
  VFS_BLOCK,
  VFS_SYMLINK,
  VFS_HARDLINK,
} vfs_file_type_t;

/* Types ==================================================================== */
/** Forward declarations of structs */
typedef struct vfs_s vfs_t;
typedef struct vfs_file_s vfs_file_t;

/**
 * Handler of 'open' operation on block file
 *
 * @param ctx Void pointer to implementation defined context
 * @param name File name
 */
typedef error_t (*vfs_block_open_fn_t)(void *, vfs_file_t *);

/**
 * Handler of 'close' operation on block file
 *
 * @param ctx Void pointer to implementation defined context
 * @param file Pointer to opened file
 */
typedef error_t (*vfs_block_close_fn_t)(void *, vfs_file_t *);

/**
 * Handler of 'read' operation on block file
 *
 * @param ctx Void pointer to implementation defined context
 * @param file Pointer to opened file
 * @param buffer Buffer to read to
 * @param size Size to read
 */
typedef error_t (*vfs_block_read_fn_t)(void *, vfs_file_t *, uint8_t *, size_t);

/**
 * Handler of 'write' operation on block file
 *
 * @param ctx Void pointer to implementation defined context
 * @param file Pointer to opened file
 * @param buffer Buffer to write from
 * @param size Size to write
 */
typedef error_t (*vfs_block_write_fn_t)(void *, vfs_file_t *, const uint8_t *, size_t);

/**
 * Handler of 'seek' operation on block file
 *
 * @param ctx Void pointer to implementation defined context
 * @param file Pointer to opened file
 * @param offset Read/write offset
 */
typedef error_t (*vfs_block_seek_fn_t)(void *, vfs_file_t *, size_t);

/**
 * Handler of 'tell' operation on block file
 *
 * @param ctx Void pointer to implementation defined context
 * @param file Pointer to opened file
 */
typedef size_t (*vfs_block_tell_fn_t)(void *, vfs_file_t *);


/**
 * Handler of 'ioctl' operation on block file
 *
 * @param ctx Void pointer to implementation defined context
 * @param file Pointer to opened file
 * @param cmd Command to execute (implementation defined)
 * @param args Vargs
 */
typedef error_t (*vfs_block_ioctl_fn_t)(void *, vfs_file_t *, int, va_list);

/**
 * VFS File structure
 *
 * File has type, name, and type-dependant state, files are chained together
 * in a linked list, which is very simple, conserves space, but it's not
 * very efficient on a large scale
 */
typedef struct vfs_file_s {
  /** Internal state */
  struct {
    struct vfs_file_s * next;
    vfs_t * vfs;
  } impl;

  /** File type */
  vfs_file_type_t type;

  /** File name */
  char name[VFS_MAX_PATH];

  /** File state, for each type of file */
  union {
    /** File state */
    struct {
      char * buffer;
      size_t size;
      size_t offset;
    } file;

    /** Block file state */
    struct {
      vfs_block_open_fn_t  open;
      vfs_block_close_fn_t close;
      vfs_block_read_fn_t  read;
      vfs_block_write_fn_t write;
      vfs_block_seek_fn_t  seek;
      vfs_block_tell_fn_t  tell;
      vfs_block_ioctl_fn_t ioctl;
      void * ctx;
    } block;

    /** Symlink state */
    struct {
      char name[VFS_MAX_PATH];
    } symlink;

    /** Hardlink state */
    struct {
      struct vfs_file_s * file;
    } hardlink;
  };
} vfs_file_t;

/**
 * VFS Context, represents a file system
 *
 * File systems are chained together in a linked list, which is very
 * simple, conserves space, but it's not very efficient on a large scale
 */
typedef struct vfs_s {
  /** Internal state */
  struct {
    struct vfs_s * next;
  } impl;

  /** Mount point path */
  char mount_point[VFS_MAX_PATH];

  /** Painter to first file (files are chained together in a linked list) */
  vfs_file_t * files;
} vfs_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes a file system
 *
 * @param vfs VFS Context
 */
error_t vfs_init(vfs_t * vfs);

/**
 * Mounts a file system to root
 *
 * @param root Root VFS
 * @param vfs VFS Context
 */
error_t vfs_mount(vfs_t * root, vfs_t * vfs);

/**
 * Unmounts a file system from root
 *
 * @param root Root VFS
 * @param vfs VFS Context
 */
error_t vfs_unmount(vfs_t * root, vfs_t * vfs);

/**
 * Binds file context to VFS context, making file a part of fs
 *
 * @param root Root VFS
 * @param file File context
 */
error_t vfs_create(vfs_t * root, vfs_file_t * file);

/**
 * Removes file context record from VFS context
 *
 * @param root Root VFS
 * @param file File context
 */
error_t vfs_remove(vfs_t * root, vfs_file_t * file);

/**
 * Renames file
 *
 * @param root Root VFS
 * @param old_name Old filename
 * @param new_name New filename
 */
error_t vfs_rename(vfs_t * root, const char * old_name, const char * new_name);

/**
 * Opens a file (returns a handle)
 *
 * @param root Root VFS
 * @param filename Filename
 *
 * @retval VFS file handle
 */
vfs_file_t * vfs_open(vfs_t * root, const char * filename);

/**
 * Closes a file
 *
 * @param root Root VFS
 * @param file File handle
 */
error_t vfs_close(vfs_t * root, vfs_file_t * file);

/**
 * Writes to an opened file
 *
 * @param file File handle
 * @param buffer Buffer to write from
 * @param size Size to write
 */
error_t vfs_write(vfs_file_t * file, const uint8_t * buffer, size_t size);

/**
 * Reads from an opened file
 *
 * @param file File handle
 * @param buffer Buffer to read to
 * @param size Size to read
 */
error_t vfs_read(vfs_file_t * file, uint8_t * buffer, size_t size);

/**
 * Sets read/write offset of the file
 *
 * @param file File handle
 * @param offset Offset to set
 */
error_t vfs_seek(vfs_file_t * file, size_t offset);

/**
 * Returns read/write offset of the file
 *
 * @param file File handle
 */
size_t vfs_tell(vfs_file_t * file);

/**
 * Performs an IOCTL operation on a file
 *
 * @param file File handle
 * @param cmd Command to execute
 * @param args Command arguments
 */
error_t vfs_ioctl(vfs_file_t * file, int cmd, va_list args);

/**
 * Finds VFS context, to which the file with filename corresponds
 *
 * @param root Root VFS handle
 * @param filename Filename
 *
 * @retval VFS handle
 */
vfs_t * vfs_find_by_file_prefix(vfs_t * root, const char * filename);

/**
 * Finds file across filesystems, given root VFS handle
 *
 * @param root Root VFS handle
 * @param filename Filename
 *
 * @retval File handle
 */
vfs_file_t * vfs_find_file(vfs_t * root, const char * filename);

#ifdef __cplusplus
}
#endif