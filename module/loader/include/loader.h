/**
 * @brief Shared (PIC) Module Loader
 *
 * Supports 2 container types: ELF & MACHO
 * Each can be enabled/disabled with WITH_LOADER_<name>_CONTAINER features
 *
 * Works by patching relocation table (based on executable container format)
 * and inserting missing symbols (more in loader/export.h)
 *
 * loader_file_load/loader_file_unload are intentionally __WEAK for them
 * to be redefined for each platform as needed
 */
#pragma once

#include <lds.h>
#include <util.h>
#include <error.h>
#include <stddef.h>

/**
 * @brief Declares an API descriptor for a specific executable container format
 */
#define LOADER_DECLARE(__name, ...) \
  LDS_APPEND_NAMED(loader_api, UTIL_CAT(__name, _loader_api), __VA_ARGS__)

typedef struct loader_file loader_file_t;

/**
 * @brief API for executable container parser
 */
typedef struct loader_api {
  bool    (*probe)(loader_file_t * file);
  error_t (*load)(loader_file_t * file);
  void *  (*sym_lookup)(loader_file_t * file, const char * name);
} loader_api_t;

/**
 * @brief API descriptors are static, so they live in an LDS
 */
LDS_DECLARE(loader_api, loader_api_t);

/**
 * @brief External symbol resolver
 *
 * While the purpose of loader_file_symbol_lookup is to resolve
 * a symbol address (by name) from inside a loadable module,
 * the purpose of loader_symbol_resolver_t is to provide
 * exported API to the module.
 *
 * So with this 2 functions, loadable module and loader application
 * can speak with each other by sharing symbols in both directions.
 */
typedef void* (*loader_symbol_resolver_t)(const char * name);

/**
 * @brief Module descriptor
 */
typedef struct loader_file {
  void * data; // actual executable (container) data
  size_t size; // executable data size

  loader_symbol_resolver_t resolver; // Resolver for external symbols
  const loader_api_t *     api;      // Container API
} loader_file_t;

/**
 * @brief Load a module (patch relocations, prepare everything)
 */
error_t loader_file_load(loader_file_t * file, void * data, size_t size);

/**
 * @brief Unload a module
 */
error_t loader_file_unload(loader_file_t * file);

/**
 * @brief Lookup a symbol from within a module by name, returning an address
 */
void * loader_file_symbol_lookup(loader_file_t * file, const char * name);
