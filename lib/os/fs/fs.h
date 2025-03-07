/** ========================================================================= *
 *
 * @file fs.h
 * @date 28-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief General FS API
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include <stdint.h>
#include <stddef.h>

#if USE_OS_FS_VFS_BACKEND
#include "vfs/vfs.h"
#endif

/* Defines ================================================================== */
#define USE_EXPERIMENTAL_FS 0

/* Macros =================================================================== */
/* Enums ==================================================================== */
typedef enum {
  OS_IOCTL_RESERVED = 0,

  // Commands for specific needs
  OS_IOCTL_RESET_DEVICE = 128,
  OS_IOCTL_RESET_DEVICE_DEFERRED,
  OS_IOCTL_WRITE_DETECTED,
  OS_IOCTL_WRITE_DETECTED_CLEAR,
  OS_IOCTL_READ_TIMEOUT_ENABLE,

  // For custom IOCTL commands
#ifdef OS_IOCTL_CMD_PORT
  OS_IOCTL_CMD_PORT
#endif
} os_ioctl_cmd_t;

/* Types ==================================================================== */
#if USE_OS_FS_VFS_BACKEND
/**
 * Typedef to VFS file
 */
#if !USE_EXPERIMENTAL_FS
typedef vfs_file_t os_file_t;
#endif

/**
 * Typedef to FS context
 */
#if !USE_EXPERIMENTAL_FS
typedef vfs_t fs_t;
#endif
#else
#error "Unsupported SYS FS backend"
#endif

#if USE_EXPERIMENTAL_FS
typedef struct os_fs_s os_file_t;
typedef struct os_fs_s os_fs_t;

typedef struct os_fs_drv_s {
  error_t (*mount)(const char *, os_fs_t *);
  error_t (*unmount)(os_fs_t *);
  error_t (*create)(const char * path, os_file_t * file);
} os_fs_drv_t;

typedef struct of_fs_s {
  os_fs_drv_t * drv;
  void * ctx;
} of_fs_t;

typedef struct os_fs_s {
  void * ctx;
} os_file_t;
#endif

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Mounts VFS to a path
 *
 * @param path Path of a mount point
 * @param vfs VFS handle to mount
 */
error_t os_mount(const char * path, fs_t * fs);

/**
 * Unmounts VFS
 *
 * @param path Path of FS mount point
 */
error_t os_unmount(const char * path);

/**
 * Creates file (binds file handle to file ostem)
 *
 * @param path Path to bind file to
 * @param file File to create
 */
error_t os_create(const char * path, os_file_t * file);

/**
 * Removes file
 *
 * @param path Path of the file
 */
error_t os_remove(const char * path);

/**
 * Renames file
 *
 * @param old_name Old filename
 * @param new_name New filename
 */
error_t os_rename(const char * old_name, const char * new_name);

/**
 * Opens file
 *
 * @param path Path of the file
 */
os_file_t * os_open(const char * path);

/**
 * Closes opened file
 *
 * @param file File handle
 */
error_t os_close(os_file_t * file);

/**
 * Writes buffer to opened file
 *
 * @param file File handle
 * @param buffer Buffer to write
 * @param size Size to write
 */
error_t os_write(os_file_t * file, const uint8_t * buffer, size_t size);

/**
 * Reads buffer from opened file
 *
 * @param file File handle
 * @param buffer Buffer to read to
 * @param size Size to read
 */
error_t os_read(os_file_t * file, uint8_t * buffer, size_t size);

/**
 * Sets read/write offset
 *
 * @param file File handle
 * @param offset Offset to write
 */
error_t os_seek(os_file_t * file, size_t offset);

/**
 * Returns read/write offset
 *
 * @param file File handle
 */
size_t os_tell(os_file_t * file);

/**
 * Performs IOCTL command
 *
 * @param file File handle
 * @param cmd Command to perform
 * @param ... Command args
 */
error_t os_ioctl(os_file_t * file, int cmd, ...);

#ifdef __cplusplus
}
#endif