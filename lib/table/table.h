/** ========================================================================= *
 *
 * @file table.h
 * @date 27-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Hash table data structure
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/compiler.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Defines table context with name and capacity
 *
 * Technically defines 2 variables - a table buffer and a table itself and
 * initializes both
 */
#define TABLE_DEFINE(name, cap)                       \
  table_node_t UTIL_CAT(name, _table_buf)[cap] = {0}; \
  table_t name = {                                    \
    .nodes = UTIL_CAT(name, _table_buf),              \
    .capacity = cap,                                  \
    .size = 0                                         \
  }

/* Enums ==================================================================== */
/* Types ==================================================================== */
/** Typedef for a hash type */
typedef uint32_t table_hash_t;

/** Node for a hash table */
typedef __PACKED_STRUCT {
  table_hash_t  hash;
  void *        value;
  bool          used;
} table_node_t;

/** Hash table context */
typedef struct {
  table_node_t * nodes;
  size_t capacity;
  size_t size;
} table_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes a table
 *
 * @param table Pointer to table context
 * @param nodes_buffer Buffer for nodes to reside in
 * @param cap Capacity of the buffer
 */
error_t table_init(table_t * table, table_node_t * nodes_buffer, size_t cap);

/**
 * Deinitializes a table
 *
 * @param table Pointer to table context
 */
error_t table_deinit(table_t * table);

/**
 * Returns table capacity
 *
 * @param table Pointer to table context
 */
size_t table_get_capacity(table_t * table);

/**
 * Returns table size
 *
 * @param table Pointer to table context
 */
size_t table_get_size(table_t * table);

/**
 * Adds a value to the table by hash
 *
 * @param table Pointer to table context
 * @param hash Hash of the value
 * @param value to add
 */
error_t table_add(table_t * table, table_hash_t hash, void * value);

/**
 * Removes value from the table by hash
 *
 * @param table Pointer to table context
 * @param hash Hash of the value
 */
error_t table_remove(table_t * table, table_hash_t hash);

/**
 * Searches the table for value which matches the hash
 *
 * @param table Pointer to table context
 * @param hash Hash of the value to find
 */
void * table_find(table_t * table, table_hash_t hash);

/**
 * Returns a hash for a string
 *
 * @param str String pointer
 */
table_hash_t table_str_hash(const char * str);

/**
 * Adds a value to the table with string key
 *
 * @param table Pointer to table context
 * @param str String key
 * @param data Value to add
 */
error_t table_add_str(table_t * table, const char * str, void * data);

/**
 * Removes a value from the table with string key
 *
 * @param table Pointer to table context
 * @param str String key
 */
error_t table_remove_str(table_t * table, const char * str);

/**
 * Finds a value in the table with string key
 *
 * @param table Pointer to table context
 * @param str String key
 */
void * table_find_str(table_t * table, const char * str);

#ifdef __cplusplus
}
#endif
