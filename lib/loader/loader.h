/** ========================================================================= *
 *
 * @file loader.h
 * @date 13-03-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief PIC Executable loader
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/util.h"
#include <stdint.h>
#include <stddef.h>
#include "elf/elf.h"

/* Defines ================================================================== */
/**
 * Internal macro
 * Defines exported api type (for iterators, section start/end markers, etc)
 */
#define __MODULE_API_TYPE const module_exported_api_t *

/**
 * Exports function/variable, for it to be accessed from modules
 *
 * Example:
 * @code{.c}
 *   int test_func(int i) { return i * i; }
 *   MODULE_EXPORT_API(test_func);
 * @endcode
 *
 * @param __name Function/variable name (without quotes)
 * @param ... Address, if commited, address of __name will be used
 */
#define MODULE_EXPORT_API(__name, ...)                                          \
  __USED const module_exported_api_t UTIL_CAT(exported_api_, __name)            \
    __SECTION(dynamic_api) = {                                                  \
      #__name, UTIL_IF_EMPTY(__VA_ARGS__, __name, __VA_ARGS__)                  \
    }

/**
 * Iterate over exported APIs
 *
 * Example:
 * @code{.c}
 *   MODULE_ITER_APIS(api) {
 *     printf("Name: %s, addr: %p\n", api->name, api->addr);
 *   }
 * @endcode
 *
 * @param __iter
 */
#define MODULE_ITER_APIS(__iter)                                                \
  extern __MODULE_API_TYPE __start_dynamic_api;                                 \
  extern __MODULE_API_TYPE __end_dynamic_api;                                   \
  for (__MODULE_API_TYPE __iter = (__MODULE_API_TYPE) &__start_dynamic_api;     \
      __iter != (__MODULE_API_TYPE) &__end_dynamic_api;                         \
      __iter++)

/* Macros =================================================================== */
/* Enums ==================================================================== */
/**
 * Module File Types
 */
typedef enum {
  EXE_TYPE_NONE = 0,
  EXE_TYPE_ELF,
} module_file_type_t;

/* Types ==================================================================== */
/**
 * Container for exported APIs
 *
 * Used by MODULE_EXPORT_API & MODULE_ITER_APIS
 */
typedef struct  {
  const char *  name;
  void *        addr;
} module_exported_api_t;

/**
 * Module context
 */
typedef struct {
  module_file_type_t type;

  union {
    elf_t elf;
  };
} module_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Loads module
 *
 * Auto-detects file type of file pointed to by data and calls corresponding
 * init function.
 *
 * For ELF, will call elf_parse and then process relocations
 *
 * @param module Module context
 * @param data Executable file data
 */
error_t module_load(module_t * module, uint8_t * data);

/**
 * Retrieves symbol from loaded module
 *
 * Can be used to import APIs from a module
 *
 * @param module Module context
 * @param symbol Symbol address will be placed into `*symbol`
 * @param name Symbol name
 */
error_t module_get_symbol(module_t * module, void ** symbol, const char * name);

#ifdef __cplusplus
}
#endif