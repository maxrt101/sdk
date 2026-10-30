#include <loader.h>

#if WITH_LOADER_ELF_CONTAINER

#include <elf.h>

#include <stdint.h>
#include <string.h>

#include <log/optional.h>

LOGGER_USE(loader);

__STATIC_INLINE error_t elf_patch_dynamic_symbols(loader_file_t * file) {
  Elf32_Ehdr * ehdr = (Elf32_Ehdr *) file->data;
  Elf32_Phdr * phdr = (Elf32_Phdr *) ((uint8_t *)file->data + ehdr->e_phoff);
  Elf32_Dyn  * dyn = NULL;

  // Locate the dynamic segment
  for (int i = 0; i < ehdr->e_phnum; i++) {
    if (phdr[i].p_type == PT_DYNAMIC) {
      dyn = (Elf32_Dyn *) (file->data + phdr[i].p_offset);
      break;
    }
  }

  if (!dyn) return E_OK;

  Elf32_Rel * rel_plt = NULL;
  size_t rel_plt_size = 0;
  Elf32_Sym * dynsym = NULL;
  const char * dynstr = NULL;

  // Extract dynamic linker tables
  for (Elf32_Dyn * d = dyn; d->d_tag != DT_NULL; d++) {
    switch (d->d_tag) {
      case DT_JMPREL:   rel_plt = (Elf32_Rel *) (file->data + d->d_un.d_ptr); break;
      case DT_PLTRELSZ: rel_plt_size = d->d_un.d_val; break;
      case DT_SYMTAB:   dynsym = (Elf32_Sym *) (file->data + d->d_un.d_ptr); break;
      case DT_STRTAB:   dynstr = (const char *) (file->data + d->d_un.d_ptr); break;
    }
  }

  if (!rel_plt || !dynsym || !dynstr) return E_OK;

  for (size_t i = 0; i < rel_plt_size / sizeof(Elf32_Rel); i++) {
    Elf32_Word sym_index = ELF32_R_SYM(rel_plt[i].r_info);
    Elf32_Word type = ELF32_R_TYPE(rel_plt[i].r_info);

    Elf32_Sym * symbol = &dynsym[sym_index];
    const char * symbol_name = &dynstr[symbol->st_name];

    Elf32_Addr resolved_addr = 0;

    if (symbol->st_shndx != SHN_UNDEF) {
      // Symbol is defined locally within the ELF
      resolved_addr = (Elf32_Addr) file->data + symbol->st_value;
    } else {
      // Symbol is undefined, look it up externally
      resolved_addr = (Elf32_Addr) file->resolver(symbol_name);
      if (!resolved_addr) {
        log_error("Undefined symbol: %s", symbol_name);
        return E_NOTFOUND;
      }
    }

    Elf32_Addr * patch_addr = (Elf32_Addr *) (file->data + rel_plt[i].r_offset);
    symbol->st_value = resolved_addr; // Update symbol value

    switch (type) {
      case R_ARM_JUMP_SLOT:  // Function calls
      case R_ARM_GLOB_DAT:   // Global variables
        *patch_addr = resolved_addr;
        break;

      case R_ARM_ABS32:      // Direct address references
        *patch_addr += resolved_addr;
        break;

      case R_ARM_RELATIVE:   // PIC: Add base address
        *patch_addr += (Elf32_Addr) file->data;
        break;

      default:
        log_error("Unsupported relocation type: %d", type);
        return E_NOTIMPL;
    }

#if 0
    log_info("Patching %s with %p at 0x%x", symbol_name, (void *) resolved_addr, rel_plt[i].r_offset);
#endif
  }

  return E_OK;
}

static bool elf_probe(loader_file_t * file) {
  uint8_t * magic = (uint8_t *)file->data;
  return (magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F');
}

static error_t elf_load(loader_file_t * file) {
  return elf_patch_dynamic_symbols(file);
}

static void * elf_sym_lookup(loader_file_t * file, const char * name) {
  Elf32_Ehdr * ehdr = (Elf32_Ehdr *) file->data;
  Elf32_Shdr * shdr = (Elf32_Shdr *) (file->data + ehdr->e_shoff);

  // Iterate over section headers to find DYNSYM or SYMTAB
  for (int i = 0; i < ehdr->e_shnum; i++) {
    if (shdr[i].sh_type == SHT_DYNSYM || shdr[i].sh_type == SHT_SYMTAB) {
      Elf32_Sym * syms = (Elf32_Sym *) (file->data + shdr[i].sh_offset);
      const char * strs = (const char *) (file->data + shdr[shdr[i].sh_link].sh_offset);
      size_t num_syms = shdr[i].sh_size / sizeof(Elf32_Sym);

      for (size_t j = 0; j < num_syms; j++) {
        if (syms[j].st_name && !strcmp(&strs[syms[j].st_name], name)) {
          return (void *) (file->data + syms[j].st_value);
        }
      }
    }
  }

  return NULL;
}

LOADER_DECLARE(elf, {
  .probe      = elf_probe,
  .load       = elf_load,
  .sym_lookup = elf_sym_lookup,
});

#endif