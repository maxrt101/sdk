/** ========================================================================= *
 *
 * @file vfs.c
 * @date 28-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief VFS Implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "vfs/vfs.h"
#include "os/alloc/alloc.h"
#include "error/assertion.h"
#include "util/util.h"
#include "log/log.h"
#include <string.h>

/* Defines ================================================================== */
#if USE_TEST
#ifdef __STATIC_INLINE
#undef __STATIC_INLINE
#endif

#define __STATIC_INLINE
#endif

/* Macros ============================================`======================= */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/**
 * Resets Node Pool
 */
__STATIC_INLINE error_t vfs_node_pool_reset(vfs_node_pool_t * pool) {
  ASSERT_RETURN(pool, E_NULL);

  memset(pool->nodes, 0, sizeof(vfs_node_container_t) * pool->size);

  return E_OK;
}

/**
 * Node Allocate
 *
 * Pointer to allocated node will be placed in *node
 */
__STATIC_INLINE error_t vfs_node_pool_alloc(vfs_node_pool_t * pool, vfs_node_t ** node) {
  ASSERT_RETURN(pool && node, E_NULL);

  for (size_t i = 0; i < pool->size; ++i) {
    if (!pool->nodes[i].used) {
      pool->nodes[i].used = true;
      *node = &pool->nodes[i].node;
      return E_OK;
    }
  }

  return E_NOMEM;
}

/**
 * Node Free
 */
__STATIC_INLINE error_t vfs_node_pool_free(vfs_node_pool_t * pool, vfs_node_t * node) {
  ASSERT_RETURN(pool, E_NULL);

  for (size_t i = 0; i < pool->size; ++i) {
    if (&pool->nodes[i].node == node) {
      pool->nodes[i].used = false;
      return E_OK;
    }
  }

  return E_NOTFOUND;
}

/**
 * Resets Table Pool
 */
__STATIC_INLINE error_t vfs_table_pool_reset(vfs_table_pool_t * pool) {
  ASSERT_RETURN(pool, E_NULL);

  memset(pool->tables, 0, sizeof(vfs_table_container_t) * pool->size);

  return E_OK;
}

/**
 * Table Allocate
 *
 * Pointer to allocated table will be placed in *table
 */
__STATIC_INLINE error_t vfs_table_pool_alloc(vfs_table_pool_t * pool, table_t ** table) {
  ASSERT_RETURN(pool && table, E_NULL);

  for (size_t i = 0; i < pool->size; ++i) {
    if (!pool->tables[i].used) {
      pool->tables[i].used = true;
      ERROR_CHECK_RETURN(table_init(&pool->tables[i].table, pool->tables[i].nodes, VFS_MAX_FOLDER_CHILDREN));
      *table = &pool->tables[i].table;
      return E_OK;
    }
  }

  return E_NOMEM;
}

/**
 * Table Free
 */
__STATIC_INLINE error_t vfs_table_pool_free(vfs_table_pool_t * pool, table_t * table) {
  ASSERT_RETURN(pool, E_NULL);

  for (size_t i = 0; i < pool->size; ++i) {
    if (&pool->tables[i].table == table) {
      pool->tables[i].used = false;
      return E_OK;
    }
  }

  return E_NOTFOUND;
}

/**
 * Meta node allocate function, that will either allocate from pool, of
 * dynamically if pool is NULL
 */
__STATIC_INLINE error_t vfs_node_alloc(vfs_t * vfs, vfs_node_t ** node) {
  ASSERT_RETURN(vfs, E_NULL);

  if (vfs->node_pool) {
    return vfs_node_pool_alloc(vfs->node_pool, node);
  } else {
    *node = VFS_ALLOC(sizeof(vfs_node_t));
    ASSERT_RETURN(*node, E_NOMEM);
  }

  return E_OK;
}

/**
 * Meta node free
 */
__STATIC_INLINE error_t vfs_node_free(vfs_t * vfs, vfs_node_t * node) {
  ASSERT_RETURN(vfs, E_NULL);

  if (vfs->node_pool) {
    return vfs_node_pool_free(vfs->node_pool, node);
  } else {
    VFS_FREE(node);
  }

  return E_OK;
}

/**
 * Meta table allocate function, that will either allocate from pool, of
 * dynamically if pool is NULL
 */
__STATIC_INLINE error_t vfs_table_alloc(vfs_t * vfs, table_t ** table) {
  ASSERT_RETURN(vfs, E_NULL);

  if (vfs->table_pool) {
    return vfs_table_pool_alloc(vfs->table_pool, table);
  } else {
    *table = VFS_ALLOC(sizeof(table_t));
    ASSERT_RETURN(*table, E_NOMEM);

    table_node_t * children = VFS_ALLOC(sizeof(table_node_t) * VFS_MAX_FOLDER_CHILDREN);
    ASSERT_RETURN(children, E_NOMEM);

    ERROR_CHECK_RETURN(table_init(*table, children, VFS_MAX_FOLDER_CHILDREN));
  }

  return E_OK;
}

/**
 * Meta table free
 */
__STATIC_INLINE error_t vfs_table_free(vfs_t * vfs, table_t * table) {
  ASSERT_RETURN(vfs, E_NULL);

  if (vfs->table_pool) {
    return vfs_table_pool_free(vfs->table_pool, table);
  } else {
    VFS_FREE(table->nodes);
    VFS_FREE(table);
  }

  return E_OK;
}

/**
 * Generic node name setter
 */
__STATIC_INLINE error_t vfs_set_node_name(vfs_node_t * node, const char * name) {
  ASSERT_RETURN(node && name, E_NULL);

  switch (node->head.type) {
    case VFS_FOLDER:
      UTIL_STR_COPY(node->folder.name, name, VFS_MAX_NAME);
    break;

    case VFS_FILE:
      UTIL_STR_COPY(node->file.name, name, VFS_MAX_NAME);
    break;

    case VFS_BLOCK:
      UTIL_STR_COPY(node->block.name, name, VFS_MAX_NAME);
    break;

    case VFS_SYMLINK:
      UTIL_STR_COPY(node->symlink.name, name, VFS_MAX_NAME);
    break;

    case VFS_HARDLINK:
      UTIL_STR_COPY(node->hardlink.name, name, VFS_MAX_NAME);
    break;

    default:
      return E_INVAL;
  }

  return E_OK;
}

/**
 * Finds node by given path
 *
 * Cornerstone of half of APIs in VFS
 */
__STATIC_INLINE vfs_node_t * vfs_find_node(vfs_t * vfs, const char * path) {
  ASSERT_RETURN(vfs && path, NULL);

  // Split path into tokens
  vfs_path_split_ctx_t split;
  ASSERT_RETURN(vfs_path_split(path, &split) == E_OK, NULL);

  // Return NULL if no tokens present
  if (!split.tokens.size) {
    return NULL;
  }

  // Temporary node - initialize with vfs root node
  vfs_node_t * tmp = &vfs->root;

  // Iterate over tokens
  for (size_t i = 0; i < split.tokens.size; ++i) {
    // Return NULL if node is invalid
    if (!tmp) {
      return NULL;
    }

    // If current node is folder
    if (tmp->head.type == VFS_FOLDER) {
      // Find next node from folder children
      tmp = table_find_str(tmp->folder.children, split.tokens.buffer[i]);
    } else {
      return NULL;
    }
  }

  return tmp;
}

/**
 * Given a path, retrieves parent node and file name
 *
 * Used in vfs_create_* functions
 */
__STATIC_INLINE error_t vfs_find_parent_and_name(vfs_t * vfs, const char * path, vfs_node_t ** parent, char * name) {
  ASSERT_RETURN(vfs && path, E_NULL);

  char tmp_path[VFS_MAX_PATH];
  UTIL_STR_COPY(tmp_path, path, VFS_MAX_PATH);

  if (vfs_path_parent(tmp_path) == E_UNDERFLOW) {
    *parent = &vfs->root;
  } else {
    *parent = vfs_find_node(vfs, tmp_path);
  }

  if (!*parent) {
    return E_NOTFOUND;
  }

  if ((*parent)->head.type != VFS_FOLDER) {
    return E_INVAL;
  }

  UTIL_STR_COPY(tmp_path, path, VFS_MAX_PATH);
  ERROR_CHECK_RETURN(vfs_path_name(tmp_path));
  UTIL_STR_COPY(name, tmp_path, VFS_MAX_NAME);

  return E_OK;
}

/**
 * Resolved link, if node is a link, otherwise returns node as-is
 */
__STATIC_INLINE vfs_node_t * vfs_resolve_link(vfs_t * vfs, vfs_node_t * node) {
  ASSERT_RETURN(node, NULL);

  if (node->head.type == VFS_SYMLINK) {
    return vfs_find_node(vfs, node->symlink.path);
  } else if (node->head.type == VFS_HARDLINK) {
    return node->hardlink.node;
  } else {
    return node;
  }
}

/**
 * Generic Node Initialization (without node-specific data)
 */
__STATIC_INLINE error_t vfs_node_init(vfs_node_t * node, vfs_node_type_t type, const char * name, bool allocated) {
  ASSERT_RETURN(node && name, E_NULL);

  node->head.type = type;

  if (allocated) {
    node->head.flags |= VFS_NODE_FLAG_ALLOCATED;
  }

  vfs_set_node_name(node, name);

  return E_OK;
}

/**
 * Recursively deinitialize node and it's children, if any
 *
 * If node was dynamically allocated - releases allocated resources
 */
__STATIC_INLINE error_t vfs_node_deinit(vfs_t * vfs, vfs_node_t * node) {
  ASSERT_RETURN(node, E_NULL);

  if (node->head.type == VFS_FOLDER) {
    for (size_t i = 0; i < node->folder.children->capacity; ++i) {
      if (node->folder.children->nodes[i].used) {
        ERROR_CHECK_RETURN(vfs_node_deinit(vfs, node->folder.children->nodes[i].value));
      }
    }

    if (node->folder.flags.allocated) {
      ERROR_CHECK_RETURN(vfs_table_free(vfs, node->folder.children));
    }
  }

#if VFS_USE_DYNAMIC_FILE_ALLOC
  if (node->head.type == VFS_FILE && node->file.data.flags.allocated) {
    VFS_FREE(node->file.data.buffer);
  }
#endif

  if (node->head.flags & VFS_NODE_FLAG_ALLOCATED) {
    ERROR_CHECK_RETURN(vfs_node_free(vfs, node));
  }

  return E_OK;
}

/**
 * Common part of vfs_create_file*
 */
__STATIC_INLINE error_t vfs_create_file_common(vfs_t * vfs, const char * path, const vfs_file_data_t * data) {
  vfs_node_t * node = vfs_find_node(vfs, path);

  ASSERT_RETURN(node, E_FAILED);

  memcpy(&node->file.data, data, sizeof(*data));

#if VFS_USE_DYNAMIC_FILE_ALLOC
  if (!node->file.data.buffer) {
    node->file.data.buffer = VFS_ALLOC(node->file.data.capacity);

    // If allocation failed - remove file & return error
    ASSERT_OR_ELSE(node->file.data.buffer,
      vfs_remove(vfs, path);
      return E_NOMEM);

    node->file.data.flags.allocated = true;
    node->file.data.size = 0;
  }
#endif

  return E_OK;
}

/**
 * Common part of vfs_create_folder*
 */
__STATIC_INLINE error_t vfs_create_block_common(vfs_t * vfs, const char * path, const vfs_block_data_t * data) {
  vfs_node_t * node = vfs_find_node(vfs, path);

  ASSERT_RETURN(node, E_FAILED);

  memcpy(&node->block.data, data, sizeof(*data));

  return E_OK;
}

/**
 * Common part of vfs_create_symlink*
 */
__STATIC_INLINE error_t vfs_create_symlink_common(vfs_t * vfs, const char * path, const char * link) {
  vfs_node_t * node = vfs_find_node(vfs, path);

  ASSERT_RETURN(node, E_FAILED);

  UTIL_STR_COPY(node->symlink.path, link, VFS_MAX_PATH);

  return E_OK;
}

/**
 * Common part of vfs_create_hardlink*
 */
__STATIC_INLINE error_t vfs_create_hardlink_common(vfs_t * vfs, const char * path, vfs_file_t * link_node) {
  vfs_node_t * node = vfs_find_node(vfs, path);

  ASSERT_RETURN(node, E_FAILED);

  node->hardlink.node = link_node;

  return E_OK;
}

/* Shared functions ========================================================= */
error_t vfs_path_concat(char * dest, const char * src, size_t max_size) {
  ASSERT_RETURN(dest && src, E_NULL);
  ASSERT_RETURN(max_size, E_INVAL);

  size_t dest_size = strlen(dest);
  size_t src_size = strlen(src);

  // Check for size overflow
  // Reserve 1 byte for path separator and 1 byte for null terminator
  if (dest_size + src_size >= max_size - 2) {
    return E_OVERFLOW;
  }

  // dest_append_idx - index into dest, where src will be appended
  // src_start_idx - index into src, from which copying will start
  size_t dest_append_idx, src_start_idx;

  if (dest[dest_size-1] != VFS_PATH_SEP && src[0] != VFS_PATH_SEP) {
    // If no path separators are present at the end of dest and start of
    // str - add separator
    dest[dest_size] = VFS_PATH_SEP;
    dest_append_idx = dest_size + 1;
    src_start_idx = 0;
  } else if (dest[dest_size-1] == VFS_PATH_SEP && src[0] == VFS_PATH_SEP) {
    // If dest ends with and src starts with separator, skip it in src
    dest_append_idx = dest_size;
    src_start_idx = 1;
  } else {
    // If either src or dest contain separator (but not at the same time)
    // indexes are set to default
    dest_append_idx = dest_size;
    src_start_idx = 0;
  }

  // Perform concatenation
  memcpy(dest + dest_append_idx, src + src_start_idx, src_size - src_start_idx);

  // Add null separator
  dest[dest_append_idx + src_size - src_start_idx + 1] = '\0';

  return E_OK;
}


error_t vfs_path_split(const char * path, vfs_path_split_ctx_t * ctx) {
  ASSERT_RETURN(path && ctx, E_NULL);

  // Calculate path size
  size_t path_size = strlen(path);

  // Copy const path to context to be able to edit it
  UTIL_STR_COPY(ctx->path, path, VFS_MAX_PATH);

  // Reset token size
  ctx->tokens.size = 0;

  // Initialize start - variable that holds start of next token
  // If first char is path separator - skip it
  const char * start = ctx->path[0] == VFS_PATH_SEP ? ctx->path + 1 : ctx->path;

  // Initialize iterator - if first char is path separator, start from 1
  size_t i = ctx->path[0] == VFS_PATH_SEP ? 1 : 0;

  // Iterate over path
  for (; i < path_size; ++i) {
    // If path separator or end of string is encountered
    if (ctx->path[i] == VFS_PATH_SEP || ctx->path[i] == '\0') {
      // Save token start marker to token list
      ctx->tokens.buffer[ctx->tokens.size++] = start;

      if (ctx->tokens.size >= VFS_MAX_PATH_DEPTH) {
        return E_OVERFLOW;
      }

      // Terminate token
      ctx->path[i] = '\0';

      // Set token start to next char
      start = &ctx->path[i+1];

      // Skip multiple path separators, if any
      while (*start && *start == VFS_PATH_SEP) {
        start++;
        i++;
      }
    }
  }

  // Terminate last token, if any
  if (ctx->path[i-1]) {
    ctx->tokens.buffer[ctx->tokens.size++] = start;
    ctx->path[i] = '\0';
  }

  return E_OK;
}

error_t vfs_path_remove_prefix(char * path, uint8_t count) {
  ASSERT_RETURN(path, E_NULL);

  vfs_path_split_ctx_t split;
  ERROR_CHECK_RETURN(vfs_path_split(path, &split));

  if (!split.tokens.size) {
    return E_EMPTY;
  }

  if (split.tokens.size <= count) {
    return E_UNDERFLOW;
  }

  size_t index = 0;

  for (size_t i = count; i < split.tokens.size; ++i) {
    size_t token_size = strlen(split.tokens.buffer[i]);
    memcpy(path + index, split.tokens.buffer[i], token_size);
    index += token_size;
    if (i + 1 != split.tokens.size) {
      path[index++] = VFS_PATH_SEP;
    }
  }

  path[index] = '\0';

  return E_OK;
}

error_t vfs_path_remove_suffix(char * path, uint8_t count) {
  ASSERT_RETURN(path, E_NULL);

  vfs_path_split_ctx_t split;
  ERROR_CHECK_RETURN(vfs_path_split(path, &split));

  if (!split.tokens.size) {
    return E_EMPTY;
  }

  if (split.tokens.size <= count) {
    return E_UNDERFLOW;
  }

  size_t index = path[0] == VFS_PATH_SEP ? 1 : 0;

  for (size_t i = 0; i < split.tokens.size - count; ++i) {
    size_t token_size = strlen(split.tokens.buffer[i]);
    memcpy(path + index, split.tokens.buffer[i], token_size);
    index += token_size;
    if (i + 1 != split.tokens.size - count) {
      path[index++] = VFS_PATH_SEP;
    }
  }

  path[index] = '\0';

  return E_OK;
}

error_t vfs_path_parent(char * path) {
  ASSERT_RETURN(path, E_NULL);

  ERROR_CHECK_RETURN(vfs_path_remove_suffix(path, 1));

  if (!strlen(path)) {
    memcpy(path, "/", 2);
  }

  return E_OK;
}

error_t vfs_path_name(char * path) {
  ASSERT_RETURN(path, E_NULL);

  vfs_path_split_ctx_t split;
  ERROR_CHECK_RETURN(vfs_path_split(path, &split));

  if (!split.tokens.size) {
    return E_EMPTY;
  }

  size_t name_size = strlen(split.tokens.buffer[split.tokens.size-1]);
  memcpy(path, split.tokens.buffer[split.tokens.size-1], name_size);
  path[name_size] = '\0';

  return E_OK;
}

size_t vfs_get_file_size(vfs_file_t * file) {
  ASSERT_RETURN(file, 0);

  if (file->head.type == VFS_FILE) {
    return file->file.data.size;
  }

  return 0;
}

error_t vfs_set_multi_open_flag(vfs_file_t * file, bool flag) {
  ASSERT_RETURN(file, E_NULL);

  if (flag) {
    file->head.flags |= VFS_NODE_FLAG_MULTI_OPEN;
  } else {
    file->head.flags &= !VFS_NODE_FLAG_MULTI_OPEN;
  }

  return E_OK;
}

const char * vfs_get_file_name(vfs_file_t * file) {
  ASSERT_RETURN(file, NULL);

  switch (file->head.type) {
    case VFS_FOLDER:    return file->folder.name;
    case VFS_FILE:      return file->file.name;
    case VFS_BLOCK:     return file->block.name;
    case VFS_SYMLINK:   return file->symlink.name;
    case VFS_HARDLINK:  return file->hardlink.name;
    default:
      return NULL;
  }
}

const char * vfs_node_type_to_string(vfs_node_type_t type) {
  switch (type) {
    case VFS_FOLDER:    return "FOLDER";
    case VFS_FILE:      return "FILE";
    case VFS_BLOCK:     return "BLOCK";
    case VFS_SYMLINK:   return "SYMLINK";
    case VFS_HARDLINK:  return "HARDLINK";
    default:
      return "NONE";
  }
}

error_t vfs_init(vfs_t * vfs, vfs_node_pool_t * node_pool, vfs_table_pool_t * table_pool) {
  ASSERT_RETURN(vfs, E_NULL);

  memset(vfs, 0, sizeof(*vfs));

  vfs->node_pool = node_pool;
  vfs->table_pool = table_pool;

  if (node_pool) {
    vfs_node_pool_reset(vfs->node_pool);
  }

  if (table_pool) {
    vfs_table_pool_reset(vfs->table_pool);
  }

  ERROR_CHECK_RETURN(vfs_node_init(&vfs->root, VFS_FOLDER, "/", false));

  table_t * table = NULL;
  ERROR_CHECK_RETURN(vfs_table_alloc(vfs, &table));
  vfs->root.folder.children = table;

  return E_OK;
}

error_t vfs_deinit(vfs_t * vfs) {
  ASSERT_RETURN(vfs, E_NULL);

  ERROR_CHECK_RETURN(vfs_node_deinit(vfs, &vfs->root));
  ERROR_CHECK_RETURN(vfs_table_free(vfs, vfs->root.folder.children));

  return E_OK;
}

error_t vfs_create(vfs_t * vfs, const char * path, vfs_file_type_t type) {
  ASSERT_RETURN(vfs && path, E_NULL);

  vfs_node_t * parent = NULL;
  char name[VFS_MAX_NAME];

  ERROR_CHECK_RETURN(vfs_find_parent_and_name(vfs, path, &parent, name));

  vfs_node_t * new_node = NULL;

  ERROR_CHECK_RETURN(vfs_node_alloc(vfs, &new_node));
  ERROR_CHECK_RETURN(vfs_node_init(new_node, type, name, true));
  ERROR_CHECK_RETURN(table_add_str(parent->folder.children, name, new_node));

  return E_OK;
}

error_t vfs_create_static(vfs_t * vfs, const char * path, vfs_file_type_t type, vfs_node_t * file) {
  ASSERT_RETURN(vfs && path && file, E_NULL);

  vfs_node_t * parent = NULL;
  char name[VFS_MAX_NAME];

  ERROR_CHECK_RETURN(vfs_find_parent_and_name(vfs, path, &parent, name));

  ERROR_CHECK_RETURN(vfs_node_init(file, type, name, false));
  ERROR_CHECK_RETURN(table_add_str(parent->folder.children, name, file));

  return E_OK;
}

error_t vfs_create_folder(vfs_t * vfs, const char * path) {
  ERROR_CHECK_RETURN(vfs_create(vfs, path, VFS_FOLDER));

  table_t * table = NULL;
  ERROR_CHECK_RETURN(vfs_table_alloc(vfs, &table));

  vfs_node_t * node = vfs_find_node(vfs, path);

  ASSERT_RETURN(node, E_NULL);

  node->folder.children = table;

  node->folder.flags.allocated = true;

  return E_OK;
}

error_t vfs_create_folder_static(vfs_t * vfs, const char * path, vfs_node_t * file) {
  return vfs_create_static(vfs, path, VFS_FOLDER, file);
}

error_t vfs_create_file(vfs_t * vfs, const char * path, const vfs_file_data_t * data) {
  ERROR_CHECK_RETURN(vfs_create(vfs, path, VFS_FILE));

  return vfs_create_file_common(vfs, path, data);
}

error_t vfs_create_file_static(vfs_t * vfs, const char * path, const vfs_file_data_t * data, vfs_node_t * file) {
  ERROR_CHECK_RETURN(vfs_create_static(vfs, path, VFS_FILE, file));

  return vfs_create_file_common(vfs, path, data);
}

error_t vfs_create_block(vfs_t * vfs, const char * path, const vfs_block_data_t * data) {
  ERROR_CHECK_RETURN(vfs_create(vfs, path, VFS_BLOCK));

  return vfs_create_block_common(vfs, path, data);
}

error_t vfs_create_block_static(vfs_t * vfs, const char * path, const vfs_block_data_t * data, vfs_file_t * file) {
  ERROR_CHECK_RETURN(vfs_create_static(vfs, path, VFS_BLOCK, file));

  return vfs_create_block_common(vfs, path, data);
}

error_t vfs_create_symlink(vfs_t * vfs, const char * path, const char * link) {
  ERROR_CHECK_RETURN(vfs_create(vfs, path, VFS_SYMLINK));

  return vfs_create_symlink_common(vfs, path, link);
}

error_t vfs_create_symlink_static(vfs_t * vfs, const char * path, const char * link, vfs_file_t * file) {
  ERROR_CHECK_RETURN(vfs_create_static(vfs, path, VFS_SYMLINK, file));

  return vfs_create_symlink_common(vfs, path, link);
}

error_t vfs_create_hardlink(vfs_t * vfs, const char * path, vfs_file_t * link_node) {
  ERROR_CHECK_RETURN(vfs_create(vfs, path, VFS_SYMLINK));

  return vfs_create_hardlink_common(vfs, path, link_node);
}

error_t vfs_create_hardlink_static(vfs_t * vfs, const char * path, vfs_file_t * link_node, vfs_file_t * file) {
  ERROR_CHECK_RETURN(vfs_create_static(vfs, path, VFS_SYMLINK, file));

  return vfs_create_hardlink_common(vfs, path, link_node);
}

error_t vfs_remove(vfs_t * vfs, const char * path) {
  ASSERT_RETURN(vfs && path, E_NULL);

  vfs_node_t * parent = NULL;
  char name[VFS_MAX_NAME];

  ERROR_CHECK_RETURN(vfs_find_parent_and_name(vfs, path, &parent, name));

  vfs_node_t * node_to_remove = vfs_find_node(vfs, path);
  ERROR_CHECK_RETURN(vfs_node_deinit(vfs, node_to_remove));

  ERROR_CHECK_RETURN(table_remove_str(parent->folder.children, name));

  return E_OK;
}

error_t vfs_rename(vfs_t * vfs, const char * path, const char * new_name) {
  ASSERT_RETURN(vfs && path && new_name, E_NULL);

  vfs_node_t * parent = NULL;
  char name[VFS_MAX_NAME];

  ERROR_CHECK_RETURN(vfs_find_parent_and_name(vfs, path, &parent, name));

  vfs_node_t * node = vfs_find_node(vfs, path);
  vfs_set_node_name(node, new_name);

  ERROR_CHECK_RETURN(table_remove_str(parent->folder.children, name));
  ERROR_CHECK_RETURN(table_add_str(parent->folder.children, new_name, node));

  return E_OK;
}

error_t vfs_move(vfs_t * vfs, const char * path, const char * new_path) {
  ASSERT_RETURN(vfs && path && new_path, E_NULL);

  // TODO: Implement

  return E_NOTIMPL;
}


error_t vfs_mkdir(vfs_t * vfs, const char * path) {
  ASSERT_RETURN(vfs && path, E_NULL);

  vfs_path_split_ctx_t split;
  ERROR_CHECK_RETURN(vfs_path_split(path, &split));

  if (!split.tokens.size) {
    return E_EMPTY;
  }

  vfs_node_t * tmp = &vfs->root;

  for (size_t i = 0; i < split.tokens.size; ++i) {
    if (tmp->head.type == VFS_FOLDER) {
      vfs_node_t * next = table_find_str(tmp->folder.children, split.tokens.buffer[i]);

      if (!next) {
        vfs_node_t * node = NULL;

        ERROR_CHECK_RETURN(vfs_node_alloc(vfs, &node));
        ERROR_CHECK_RETURN(vfs_node_init(node, VFS_FOLDER, split.tokens.buffer[i], true));

        table_t * table = NULL;
        ERROR_CHECK_RETURN(vfs_table_alloc(vfs, &table));
        node->folder.children = table;
        node->folder.flags.allocated = true;

        ERROR_CHECK_RETURN(table_add_str(tmp->folder.children, split.tokens.buffer[i], node));

        tmp = node;
      } else {
        tmp = next;
      }
    }
  }

  return E_OK;
}

vfs_file_t * vfs_open(vfs_t * vfs, const char * path) {
  ASSERT_RETURN(vfs && path, NULL);

  if (!strcmp(path, "/")) {
    return &vfs->root;
  }

  vfs_node_t * node = vfs_resolve_link(vfs, vfs_find_node(vfs, path));

  if (node->head.flags & VFS_NODE_FLAG_OPENED && !(node->head.flags & VFS_NODE_FLAG_MULTI_OPEN)) {
    return NULL;
  }

  switch (node->head.type) {
    case VFS_FILE:
      node->file.data.offset = 0;
      break;

    case VFS_BLOCK:
      if (node->block.data.open) {
        ASSERT_RETURN(node->block.data.open(node->block.data.ctx, node) == E_OK, NULL);
      }
      break;

    default:
      break;
  }

  node->head.flags |= VFS_NODE_FLAG_OPENED;

  return node;
}

error_t vfs_close(vfs_file_t * file) {
  ASSERT_RETURN(file, E_NULL);

  if (file->head.type == VFS_BLOCK) {
    if (file->block.data.close) {
      ERROR_CHECK_RETURN(file->block.data.close(file->block.data.ctx, file));
    }
  } else if (file->head.type == VFS_FILE) {
    file->file.data.offset = 0;
  }

  file->head.flags &= ~VFS_NODE_FLAG_OPENED;

  // TODO: ???

  return E_OK;
}

error_t vfs_read(vfs_file_t * file, uint8_t * buffer, size_t size) {
  ASSERT_RETURN(file && buffer && size, E_NULL);

  switch (file->head.type) {
    case VFS_FILE: {
      size_t read_size = UTIL_MIN(file->file.data.size - file->file.data.offset, size);
      void * res = memcpy(buffer, file->file.data.buffer + file->file.data.offset, read_size);
      file->file.data.offset += read_size;
      return res == buffer ? E_OK : E_FAILED;
    }

    case VFS_BLOCK: {
      if (file->block.data.read) {
        return file->block.data.read(file->block.data.ctx, file, buffer, size);
      }
      break;
    }

    default:
      break;
  }

  return E_NOTIMPL;
}

error_t vfs_write(vfs_file_t * file, const uint8_t * buffer, size_t size) {
  ASSERT_RETURN(file && buffer && size, E_NULL);

  switch (file->head.type) {
    case VFS_FILE: {
      size_t write_size = UTIL_MIN(file->file.data.capacity - file->file.data.offset, size);
      void * res = memcpy(file->file.data.buffer + file->file.data.offset, buffer, write_size);
      file->file.data.offset += write_size;
      file->file.data.size += write_size;
      return res == file->file.data.buffer + file->file.data.offset - write_size ? E_OK : E_FAILED;
    }

    case VFS_BLOCK: {
      if (file->block.data.write) {
        return file->block.data.write(file->block.data.ctx, file, buffer, size);
      }
      break;
    }

    default:
      break;
  }

  return E_NOTIMPL;
}

error_t vfs_seek(vfs_file_t * file, size_t offset) {
  ASSERT_RETURN(file, E_NULL);

  switch (file->head.type) {
    case VFS_FILE: {
      file->file.data.offset = UTIL_MIN(offset, file->file.data.size);
      return E_OK;
    }

    case VFS_BLOCK: {
      return vfs_ioctl(file, VFS_IOCTL_SEEK, offset);
    }

    default:
      break;
  }

  return E_NOTIMPL;
}

size_t vfs_tell(vfs_file_t * file) {
  ASSERT_RETURN(file, 0);

  switch (file->head.type) {
    case VFS_FILE: {
      return file->file.data.offset;
    }

    case VFS_BLOCK: {
      size_t offset = 0;
      ERROR_CHECK(vfs_ioctl(file, VFS_IOCTL_TELL, &offset), return 0);
      return offset;
    }

    default:
      break;
  }

  return 0;
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

  switch (file->head.type) {
    case VFS_BLOCK: {
      if (file->block.data.ioctl) {
        return file->block.data.ioctl(file->block.data.ctx, file, cmd, args);
      }
      break;
    }

    default:
      break;
  }

  return E_NOTIMPL;
}
