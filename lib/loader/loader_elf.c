/** ========================================================================= *
*
 * @file loader_elf.c
 * @date 13-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Backend for ELF file type in loader
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "loader/loader.h"
#include "error/assertion.h"
#include "util/util.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG loader

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
__STATIC_INLINE Elf32_Addr elf_resolve_symbol(const char * name) {
  MODULE_ITER_APIS(api) {
    if (!strcmp(api->name, name)) {
      return (Elf32_Addr) api->addr;
    }
  }

  return 0;
}

__STATIC_INLINE error_t elf_patch_dynamic_symbols(elf_t * elf) {
  Elf32_Rel * rel_plt = NULL;
  size_t rel_plt_size = 0;

  for (Elf32_Dyn * dyn = elf->dynamic.dyn; dyn->d_tag != DT_NULL; dyn++) {
    switch (dyn->d_tag) {
      case DT_JMPREL:
        rel_plt = (Elf32_Rel *)(elf->data + dyn->d_un.d_ptr);
        break;

      case DT_PLTRELSZ:
        rel_plt_size = dyn->d_un.d_val;
        break;

      default:
        break;
    }
  }

  for (size_t i = 0; i < rel_plt_size / sizeof(Elf32_Rel); i++) {
    Elf32_Word sym_index = ELF32_R_SYM(rel_plt[i].r_info);
    Elf32_Word type = ELF32_R_TYPE(rel_plt[i].r_info);

#if 0
    log_info("rel %d %d %s", i, type, elf_rel_type_to_string(type));
#endif

    Elf32_Sym * symbol = &elf->dynsym.sym[sym_index];
    const char *symbol_name = &elf->dynstr.str[symbol->st_name];

    elf_symbol_t sym;
    ERROR_CHECK_RETURN(elf_find_symbol(elf, &sym, symbol_name));

    Elf32_Addr resolved_addr = (Elf32_Addr) sym.addr;

    if (!resolved_addr) {
      resolved_addr = elf_resolve_symbol(symbol_name);

      if (!resolved_addr) {
        log_info("Error: Undefined symbol: %s", symbol_name);
        return E_NOTFOUND;
      }
    }

    Elf32_Addr * patch_addr = (Elf32_Addr *) (elf->data + rel_plt[i].r_offset);

    // Patch symbol entry with correct value
    symbol->st_value = resolved_addr;

    switch (type) {
      case R_ARM_JUMP_SLOT:  // Function calls
      case R_ARM_GLOB_DAT:   // Global variables
        *patch_addr = resolved_addr;
      break;

      case R_ARM_ABS32:      // Direct address references
        *patch_addr += resolved_addr;
      break;

      case R_ARM_RELATIVE:   // PIC: Add base address
        *patch_addr += (Elf32_Addr) elf->data;
      break;

      default:
        log_info("Unsupported relocation type: %d", type);
        return E_NOTIMPL;
    }

    log_info("Patching %s with %p at 0x%x (type=%s)",
      symbol_name, (void *) resolved_addr, rel_plt[i].r_offset, elf_rel_type_to_string(type));
  }

  return E_OK;
}

/* Shared functions ========================================================= */
error_t module_elf_load(module_t * module, uint8_t * data) {
  ERROR_CHECK_RETURN(elf_parse(&module->elf, data));
  ERROR_CHECK_RETURN(elf_patch_dynamic_symbols(&module->elf));

  return E_OK;
}

error_t module_elf_get_symbol(module_t * module, void ** symbol, const char * name) {
  elf_symbol_t sym;

  ERROR_CHECK_RETURN(elf_find_symbol(&module->elf, &sym, name));

  *symbol = sym.addr;

  return E_OK;
}
