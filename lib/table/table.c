/** ========================================================================= *
 *
 * @file table.c
 * @date 27-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Hash table data structure
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "table/table.h"
#include "error/assertion.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
__STATIC_INLINE table_node_t * table_find_node(table_t * table, table_hash_t hash) {
  ASSERT_RETURN(table, NULL);

  table_node_t * node = &table->nodes[hash % table->capacity];

  for (size_t i = 0; i < table->capacity; ++i) {
    if (node->used && node->hash == hash) {
      return node;
    }
    node = &node[i];
  }

  return NULL;
}

/* Shared functions ========================================================= */
error_t table_init(table_t * table, table_node_t * nodes_buffer, size_t cap) {
  ASSERT_RETURN(table && nodes_buffer && cap, E_NULL);

  table->nodes = nodes_buffer;
  table->capacity = cap;
  table->size = 0;

  return E_OK;
}

error_t table_deinit(table_t * table) {
  return E_NOTIMPL;
}

size_t table_get_capacity(table_t * table) {
  ASSERT_RETURN(table, 0);
  return table->capacity;
}

size_t table_get_size(table_t * table) {
  ASSERT_RETURN(table, 0);
  return table->size;
}

error_t table_add(table_t * table, table_hash_t hash, void * value) {
  ASSERT_RETURN(table, E_NULL);

  table_node_t * node = &table->nodes[hash % table->capacity];

  for (size_t i = 0; i < table->capacity; ++i) {
    if (!node->used) {
      node->hash = hash;
      node->value = value;
      node->used = true;
      table->size++;
      return E_OK;
    }
    node = &node[i];
  }

  return E_NOMEM;
}

error_t table_remove(table_t * table, table_hash_t hash) {
  ASSERT_RETURN(table, E_NULL);

  table_node_t * node = table_find_node(table, hash);

  if (!node) {
    return E_NOTFOUND;
  }

  node->used = false;
  table->size--;

  return E_OK;
}

void * table_find(table_t * table, table_hash_t hash) {
  ASSERT_RETURN(table, NULL);

  table_node_t * node = table_find_node(table, hash);

  return node ? node->value : NULL;
}

table_hash_t table_str_hash(const char * str) {
  table_hash_t result = 7;

  while (*str) {
    result = result * 31 + *str++;
  }

  return result;
}

error_t table_add_str(table_t * table, const char * str, void * data) {
  return table_add(table, table_str_hash(str), data);
}

error_t table_remove_str(table_t * table, const char * str) {
  return table_remove(table, table_str_hash(str));
}

void * table_find_str(table_t * table, const char * str) {
  return table_find(table, table_str_hash(str));
}
