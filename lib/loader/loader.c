/** ========================================================================= *
 *
 * @file loader.c
 * @date 13-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "loader/loader.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG LOADER

/* Macros =================================================================== */
#define MODULE_RUN_FOR_FILETYPE(__module, __fn, ...)                            \
  do {                                                                          \
    switch (__module->type) {                                                   \
      case EXE_TYPE_ELF:                                                        \
        return module_elf_ ## __fn (module, ## __VA_ARGS__);                    \
      case EXE_TYPE_NONE:                                                       \
      default:                                                                  \
        return E_NOTIMPL;                                                       \
    }                                                                           \
  } while (0)

/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
error_t module_elf_load(module_t * module, uint8_t * data);
error_t module_elf_get_symbol(module_t * module, void ** symbol, const char * name);

__STATIC_INLINE module_file_type_t get_file_type(uint8_t * data) {
  ASSERT_RETURN(data, EXE_TYPE_NONE);

  if (elf_check(data) == E_OK) {
    return EXE_TYPE_ELF;
  }

  return E_NOTIMPL;
}

/* Shared functions ========================================================= */
error_t module_load(module_t * module, uint8_t * data) {
  ASSERT_RETURN(module && data, E_NULL);

  module->type = get_file_type(data);

  MODULE_RUN_FOR_FILETYPE(module, load, data);
}

error_t module_get_symbol(module_t * module, void ** symbol, const char * name) {
  ASSERT_RETURN(module && symbol && name, E_NULL);

  MODULE_RUN_FOR_FILETYPE(module, get_symbol, symbol, name);
}
