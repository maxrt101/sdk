/**
 * @brief Part of loader module - used for exporting symbols into the loadable modules
 */
#pragma once

#include <lds.h>
#include <util.h>

/**
 * @brief Export a symbol into the symbol table
 *
 * By default symbol name and int's address are derived from __name,
 * but this can be redefined using optional second argument as address
 * and first argument purely as name
 *
 * Example:
 * @code{.c}
    int test() { return 42; }

    EXPORT_SYMBOL(test);
    EXPORT_SYMBOL("test_alias", test);
 * @endcode
 */
#define EXPORT_SYMBOL(__name, ...) \
  LDS_APPEND_NAMED(export_symbols, UTIL_CAT(__name, _export_symbol), {  \
    .name = UTIL_STRINGIFY(__name),                                     \
    .addr = UTIL_DEFAULT_OR_VA_ARGS(&__name, __VA_ARGS__)               \
  })

/**
 * @brief Represents an exported symbol - name + address pair
 */
typedef struct export_symbol {
  const char * name;
  void *       addr;
} export_symbol_t;

/**
 * @brief Symbol table is static, so it lives in an LDS
 */
LDS_DECLARE(export_symbols, export_symbol_t);

/**
 * @brief Lookup symbol address by name
 *
 * @returns NULL if no symbol found
 */
void * export_symbol_lookup(const char * name);
