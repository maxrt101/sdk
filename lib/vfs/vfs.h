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
#include "util/compiler.h"
#include "table/table.h"
#include "error/error.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/**
 * Include header needed for VFS_ALLOC/VFS_FREE, if exists
 */
#ifdef VFS_ALLOC_INC
#include VFS_ALLOC_INC
#endif

/* Defines ================================================================== */
/**
 * Max children a folder can have
 */
#ifndef VFS_MAX_FOLDER_CHILDREN
#define VFS_MAX_FOLDER_CHILDREN       4
#endif

/**
 * Max file name in chars
 */
#ifndef VFS_MAX_NAME
#define VFS_MAX_NAME                  16
#endif

/**
 * Max oath in chars
 */
#ifndef VFS_MAX_PATH
#define VFS_MAX_PATH                  32
#endif

/**
 * Maximum path depth (artificial constraint, because split needs a limit to
 * how many path parts it can split)
 */
#ifndef VFS_MAX_PATH_DEPTH
#define VFS_MAX_PATH_DEPTH            4
#endif

/**
 * If enabled, vfs will create and manage buffer for VFS_FILE dynamically
 * using VFS_ALLOC/VFS_FREE when creating VFS_FILE with buffer == NULL
 * and capacity != 0
 */
#ifndef VFS_USE_DYNAMIC_FILE_ALLOC
#define VFS_USE_DYNAMIC_FILE_ALLOC    1
#endif

/**
 * Allocator used by VFS
 *
 * Analogue to malloc
 */
#ifndef VFS_ALLOC
#define VFS_ALLOC                     os_alloc
#endif

/**
 * Allocator used by VFS
 *
 * Analogue for free
 */
#ifndef VFS_FREE
#define VFS_FREE                      os_free
#endif

/**
 * VFS Path separator
 *
 * Must be single character
 */
#ifndef VFS_PATH_SEP
#define VFS_PATH_SEP                  '/'
#endif

/**
 * Special value that can be passed to vfs_seek
 */
#define VFS_SEEK_END                  (-1U)

/* Macros =================================================================== */
/**
 * Declares node pool for VFS instance
 *
 * Creates 2 variables - array (pool) with node containers and pool context
 *
 * @param name Pool name. Variable for pool context will be named like this
 *             Array variable for array will be named name+_vfs_node_pool_nodes
 * @param size Size of pool in elements
 */
#define VFS_DECLARE_NODE_POOL(name, size)                                       \
  vfs_node_container_t UTIL_CAT(name, _vfs_node_pool_nodes)[size] = {0};        \
  vfs_node_pool_t name = {UTIL_CAT(name, _vfs_node_pool_nodes), size}

/**
 * Declares table pool for VFS instance
 *
 * Creates 2 variables - array (pool) with table containers and pool context
 *
 * @param name Pool name. Variable for pool context will be named like this
 *             Array variable for array will be named
 *             name+_vfs_table_pool_nodes
 * @param size Size of pool in elements
 */
#define VFS_DECLARE_TABLE_POOL(name, size)                                      \
  vfs_table_container_t UTIL_CAT(name, _vfs_table_pool_nodes)[size] = {0};      \
  vfs_table_pool_t name = {UTIL_CAT(name, _vfs_table_pool_nodes), size}

/* Enums ==================================================================== */
/**
 * VFS IOCTL Commands
 *
 * Can be expanded by defining VFS_IOCTL_CMD_PORT with needed values separated
 * by comma
 */
typedef enum {
  VFS_IOCTL_NONE = 0,

  VFS_IOCTL_SEEK = 1,
  VFS_IOCTL_TELL = 2,

  VFS_IOCTL_RESERVED_128 = 128,

#ifdef VFS_IOCTL_CMD_PORT
  VFS_IOCTL_CMD_PORT
#endif
} vfs_ioctl_cmd_t;

/**
 * VFS Node Type
 *
 * Effectively is the file type
 */
typedef enum {
  VFS_NONE     = 0,
  VFS_FOLDER   = 1,
  VFS_FILE     = 2,
  VFS_BLOCK    = 3,
  VFS_SYMLINK  = 4,
  VFS_HARDLINK = 5,
} vfs_node_type_t;

/**
 * VFS Node Flags
 *
 * For internal usage
 */
typedef enum {
  VFS_NODE_FLAG_NONE      = 0,
  VFS_NODE_FLAG_ALLOCATED = 1 << 0,
} vfs_node_flags_t;

/* Types ==================================================================== */
/**
 * Make alias node_type == file_type
 */
typedef vfs_node_type_t vfs_file_type_t;

/**
 * Forward declaration of VFS Node
 */
typedef union vfs_node_t vfs_node_t;

/**
 * Forward declaration of VFS
 */
typedef struct vfs_s vfs_t;

/**
 * Forward declaration of VFS File
 */
typedef vfs_node_t vfs_file_t;

/**
 * VFS Block File open() callback
 *
 * @param ctx User context, passed to vfs_create_block
 * @param file File, on which, the operation is performed
 */
typedef error_t (*vfs_block_open_fn_t)(void *, vfs_file_t *);

/**
 * VFS Block File close() callback
 *
 * @param ctx User context, passed to vfs_create_block
 * @param file File, on which, the operation is performed
 */
typedef error_t (*vfs_block_close_fn_t)(void *, vfs_file_t *);

/**
 * VFS Block File read() callback
 *
 * @param ctx User context, passed to vfs_create_block
 * @param file File, on which, the operation is performed
 * @param buffer Buffer to read to
 * @param size Size to read
 */
typedef error_t (*vfs_block_read_fn_t)(void *, vfs_file_t *, uint8_t *, size_t);

/**
 * VFS Block File write() callback
 *
 * @param ctx User context, passed to vfs_create_block
 * @param file File, on which, the operation is performed
 * @param buffer Buffer to write to file from
 * @param size Size to write
 */
typedef error_t (*vfs_block_write_fn_t)(void *, vfs_file_t *, const uint8_t *, size_t);

/**
 * VFS Block File ioctl() callback
 *
 * @param ctx User context, passed to vfs_create_block
 * @param file File, on which, the operation is performed
 * @param args Variadic Argument List
 */
typedef error_t (*vfs_block_ioctl_fn_t)(void *, vfs_file_t *, int, va_list);

/**
 * VFS_FILE Node data
 */
typedef __PACKED_STRUCT {
  uint8_t * buffer;
  size_t    size;
  size_t    capacity;
  size_t    offset;
  __PACKED_STRUCT {
    bool    allocated : 1;
  } flags;
} vfs_file_data_t;

/**
 * VFS_BLOCK Node data
 */
typedef __PACKED_STRUCT {
  void *               ctx;
  vfs_block_open_fn_t  open;
  vfs_block_close_fn_t close;
  vfs_block_read_fn_t  read;
  vfs_block_write_fn_t write;
  vfs_block_ioctl_fn_t ioctl;
} vfs_block_data_t;

/**
 * VFS Node common header
 *
 * Every node type starts with it
 */
typedef __PACKED_STRUCT {
  vfs_node_type_t  type  : 4;
  vfs_node_flags_t flags : 4;
  uint8_t data[0];
} vfs_node_head_t;

/**
 * VFS Folder Node
 *
 * Payload for node with VFS_FOLDER type
 */
typedef __PACKED_STRUCT {
  vfs_node_head_t head;
  char            name[VFS_MAX_NAME];
  table_t *       children;
  __PACKED_STRUCT {
    bool          allocated : 1;
  } flags;
} vfs_node_folder_t;

/**
 * VFS File Node
 *
 * Payload for node with VFS_FILE type
 */
typedef __PACKED_STRUCT {
  vfs_node_head_t head;
  char            name[VFS_MAX_NAME];
  vfs_file_data_t data;
} vfs_node_file_t;

/**
 * VFS Block Node
 *
 * Payload for node with VFS_BLOCK type
 */
typedef __PACKED_STRUCT {
  vfs_node_head_t  head;
  char             name[VFS_MAX_NAME];
  vfs_block_data_t data;
} vfs_node_block_t;

/**
 * VFS Symlink Node
 *
 * Payload for node with VFS_SYMLINK type
 */
typedef __PACKED_STRUCT {
  vfs_node_head_t  head;
  char             name[VFS_MAX_NAME];
  char             path[VFS_MAX_PATH];
} vfs_node_symlink_t;

/**
 * VFS Hardlink Node
 *
 * Payload for node with VFS_HARDLINK type
 */
typedef __PACKED_STRUCT {
  vfs_node_head_t  head;
  char             name[VFS_MAX_NAME];
  vfs_node_t *     node;
} vfs_node_hardlink_t;

/**
 * VFS Composite Node Type
 */
typedef __PACKED_UNION vfs_node_t {
  vfs_node_head_t     head;
  vfs_node_folder_t   folder;
  vfs_node_file_t     file;
  vfs_node_block_t    block;
  vfs_node_symlink_t  symlink;
  vfs_node_hardlink_t hardlink;
} vfs_node_t;

/**
 * VFS Node Container for node pool
 */
typedef struct {
  vfs_node_t node;
  bool       used;
} vfs_node_container_t;

/**
 * VFS Node Pool
 */
typedef struct {
  vfs_node_container_t * nodes;
  size_t size;
} vfs_node_pool_t;

/**
 * VFS Table Container for table pool
 */
typedef struct {
  table_t      table;
  table_node_t nodes[VFS_MAX_FOLDER_CHILDREN];
  bool         used;
} vfs_table_container_t;

/**
 * VFS Table Pool
 */
typedef struct {
  vfs_table_container_t * tables;
  size_t size;
} vfs_table_pool_t;

/**
 * VFS Context
 */
typedef struct vfs_s {
  vfs_node_t         root;

  vfs_node_pool_t *  node_pool;
  vfs_table_pool_t * table_pool;
} vfs_t;

/**
 * VFS Split Context
 *
 * Used by vfs_path_split to return divided path
 */
typedef struct {
  /**
   * Contains original path, with path separators replaced with '\0'
   */
  char path[VFS_MAX_PATH];

  struct {
    /**
     * Path tokens buffer
     */
    const char * buffer[VFS_MAX_PATH_DEPTH];

    /**
     * Path tokens buffer size
     */
    size_t size;
  } tokens;
} vfs_path_split_ctx_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Concatenates 2 paths
 *
 * @param dest Path to which src will be concatenated. Result
 * @param src Path to concatenate
 * @param max_size Size of dest
 */
error_t vfs_path_concat(char * dest, const char * src, size_t max_size);

/**
 * Splits path by VFS_PATH_SEP
 *
 * @warning Can't split into more than VFS_MAX_PATH_DEPTH tokens
 *
 * @param path Path to split
 * @param ctx Split context (ctx->tokens.buffer - separated tokens,
 *            ctx->tokens.size - number of tokens in buffer)
 */
error_t vfs_path_split(const char * path, vfs_path_split_ctx_t * ctx);

/**
 * Removes N prefix path tokens (uses vfs_path_split to tokenize)
 *
 * @param path Path to remove prefix from. Result
 * @param count How many tokens to remove
 */
error_t vfs_path_remove_prefix(char * path, uint8_t count);

/**
 * Removes N suffix path tokens (uses vfs_path_split to tokenize)
 *
 * @param path Path to remove suffix from. Result
 * @param count How many tokens to remove
 */
error_t vfs_path_remove_suffix(char * path, uint8_t count);

/**
 * Retrieves parent (all except last token) from path
 *
 * @param path Path to get parent from. Result
 */
error_t vfs_path_parent(char * path);

/**
 * Retrieves name (last token) from path
 *
 * @param path Path to get name from. Result
 */
error_t vfs_path_name(char * path);

/**
 *
 * @param file
 * @return
 */
size_t vfs_get_file_size(vfs_file_t * file);

/**
 * Retrieves name (last token) from file
 *
 * @param file File to get name from
 */
const char * vfs_get_file_name(vfs_file_t * file);

/**
 *
 * @param type
 * @return
 */
const char * vfs_node_type_to_string(vfs_node_type_t type);

/**
 * Initializes VFS instance
 *
 * @note If node/table pool is NULL - will use dynamic allocation for that
 *       resource
 *
 * @param vfs VFS Context
 * @param node_pool Node pool (@ref VFS_DECLARE_NODE_POOL). If NULL - will
 *                  use dynamic allocation
 * @param table_pool Table pool (@ref VFS_DECLARE_TABLE_POOL). If NULL - will
 *                  use dynamic allocation
 */
error_t vfs_init(vfs_t * vfs, vfs_node_pool_t * node_pool, vfs_table_pool_t * table_pool);

/**
 * Deinitializes VFS instance
 *
 * @note If node/table pool is NULL will deallocate allocated resources
 *
 * @param vfs VFS Context
 */
error_t vfs_deinit(vfs_t * vfs);

/**
 * Create blank file (node) at requested path
 *
 * @note Won't initialize create file
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to file
 * @param type File type
 */
error_t vfs_create(vfs_t * vfs, const char * path, vfs_file_type_t type);

/**
 * Statically create blank file (node) at requested path
 *
 * @note Won't initialize create file
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to file
 * @param type File type
 * @param file File (node) to use as context for newly created file
 */
error_t vfs_create_static(vfs_t * vfs, const char * path, vfs_file_type_t type, vfs_node_t * file);

/**
 * Create folder at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to folder
 */
error_t vfs_create_folder(vfs_t * vfs, const char * path);

/**
 * Statically create folder at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to folder
 * @param file File to use as context for newly created folder
 */
error_t vfs_create_folder_static(vfs_t * vfs, const char * path, vfs_node_t * file);

/**
 * Create file at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to file
 * @param data File payload data
 */
error_t vfs_create_file(vfs_t * vfs, const char * path, const vfs_file_data_t * data);

/**
 * Statically create file at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to file
 * @param data File payload data
 * @param file File to use as context for newly created folder
 */
error_t vfs_create_file_static(vfs_t * vfs, const char * path, const vfs_file_data_t * data, vfs_node_t * file);

/**
 * Create block file at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to block file
 * @param data File payload data
 */
error_t vfs_create_block(vfs_t * vfs, const char * path, const vfs_block_data_t * data);

/**
 * Statically create block file at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to block file
 * @param data File payload data
 * @param file File to use as context for newly created file
 */
error_t vfs_create_block_static(vfs_t * vfs, const char * path, const vfs_block_data_t * data, vfs_file_t * file);

/**
 * Create symlink at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to symlink
 * @param link Link target
 */
error_t vfs_create_symlink(vfs_t * vfs, const char * path, const char * link);

/**
 * Statically create symlink at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to symlink file
 * @param link Link target
 * @param file File to use as context for newly created symlink
 */
error_t vfs_create_symlink_static(vfs_t * vfs, const char * path, const char * link, vfs_file_t * file);

/**
 * Create hardlink at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to hardlink
 * @param link_node Link target
 */
error_t vfs_create_hardlink(vfs_t * vfs, const char * path, vfs_file_t * link_node);

/**
 * Statically create hardlink at requested path
 *
 * @note Won't create parent folders
 *
 * @param vfs VFS Context
 * @param path Path to hardlink file
 * @param link_node Link target
 * @param file File to use as context for newly created hardlink
 */
error_t vfs_create_hardlink_static(vfs_t * vfs, const char * path, vfs_file_t * link_node, vfs_file_t * file);

/**
 * Remove file/folder/etc. (node)
 *
 * @param vfs VFS Context
 * @param path Path to file
 */
error_t vfs_remove(vfs_t * vfs, const char * path);

/**
 * Rename file/folder/etc. (node)
 *
 * @note Won't rename path, only name
 *
 * @param vfs VFS Context
 * @param path Path to file
 * @param new_name New name
 */
error_t vfs_rename(vfs_t * vfs, const char * path, const char * new_name);

/**
 * Move file/folder/etc. (node)
 *
 * @warning Not implemented
 *
 * @param vfs VFS Context
 * @param path Path to file
 * @param new_path New path
 */
error_t vfs_move(vfs_t * vfs, const char * path, const char * new_path);

/**
 * Recursively create directory path
 *
 * @param vfs VFS Context
 * @param path Path to create
 */
error_t vfs_mkdir(vfs_t * vfs, const char * path);

/**
 * Opens file
 *
 * @param vfs VFS Context
 * @param path Path to file
 * @return NULL if failed, pointer to file if successful
 */
vfs_file_t * vfs_open(vfs_t * vfs, const char * path);

/**
 * Closes opened file
 *
 * @param file File context
 */
error_t vfs_close(vfs_file_t * file);

/**
 * Read from file
 *
 * @param file Opened file context
 * @param buffer Buffer to read to
 * @param size Read size
 */
error_t vfs_read(vfs_file_t * file, uint8_t * buffer, size_t size);

/**
 * Write to file
 *
 * @param file Opened file context
 * @param buffer Buffer to write from
 * @param size Write size
 */
error_t vfs_write(vfs_file_t * file, const uint8_t * buffer, size_t size);

/**
 * Set file read/write offset
 *
 * @param file Opened file context
 * @param offset New offset. If VFS_SEEK_END will set to file size
 */
error_t vfs_seek(vfs_file_t * file, size_t offset);

/**
 * Return current file read/write offset
 *
 * @param file Opened file context
 * @return Current file offset
 */
size_t vfs_tell(vfs_file_t * file);

/**
 * Perform IOCTL operation on file
 *
 * @note Useless on any file except VFS_BLOCK
 *
 * @param file Opened file context
 * @param cmd IOCTL Command
 * @param ... Command specific arguments
 */
error_t vfs_ioctl(vfs_file_t * file, int cmd, ...);

/**
 * Perform IOCTL operation on file
 *
 * @note Useless on any file except VFS_BLOCK
 *
 * @param file Opened file context
 * @param cmd IOCTL Command
 * @param args Command specific arguments
 */
error_t vfs_ioctl_va(vfs_file_t * file, int cmd, va_list args);

#ifdef __cplusplus
}
#endif