/** ========================================================================= *
 *
 * @file elf.c
 * @date 13-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief ELF Support & units
 *
 * TODO: Check indexing into strtab/symtab/etc. by offset
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "elf/elf.h"
#include "error/assertion.h"
#include "util/util.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG ELF

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
__STATIC_INLINE const char * null_str_guard(const char * str) {
  if (!str) {
    return "<null>";
  }

  return str;
}

__STATIC_INLINE const char * dynstr_get(elf_t * elf, uint32_t offset) {
  if (!elf->dynstr.header) {
    return NULL;
  }

  if (offset > elf->dynstr.header->sh_size) {
    return NULL;
  }

  return &elf->dynstr.str[offset];
}

/* Shared functions ========================================================= */
// TODO: error_t elf_find_sections(elf_t * elf, elf_section_t * sections, size_t size);
// TODO: error_t elf_find_symbols(elf_t * elf, elf_symbol_t * symbols, size_t size);

error_t elf_parse(elf_t * elf, uint8_t * data) {
  ASSERT_RETURN(elf && data, E_NULL);
  ERROR_CHECK_RETURN(elf_check(data));

  memset(elf, 0, sizeof(*elf));

  elf->data = data;
  elf->sections = (Elf32_Shdr *) (elf->data + elf->header->e_shoff);
  elf->shstrtab = (const char *) (elf->data + elf->sections[elf->header->e_shstrndx].sh_offset);

  // TODO: UTIL_IF_1(ELF_AUTOLOAD_STRTAB, ...)
  elf_find_section(elf, &elf->symtab,  ".symtab");
  elf_find_section(elf, &elf->strtab,  ".strtab");
  elf_find_section(elf, &elf->dynsym,  ".dynsym");
  elf_find_section(elf, &elf->dynstr,  ".dynstr");
  elf_find_section(elf, &elf->dynamic, ".dynamic");
  elf_find_section(elf, &elf->got,     ".got");

  return E_OK;
}

error_t elf_check(uint8_t * data) {
  Elf32_Ehdr * ehdr = (Elf32_Ehdr *) data;

  return (ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
          ehdr->e_ident[EI_MAG1] == ELFMAG1 &&
          ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
          ehdr->e_ident[EI_MAG3] == ELFMAG3) ? E_OK : E_CORRUPT;
}

error_t elf_find_section(elf_t * elf, elf_section_t * section, const char * name) {
  ASSERT_RETURN(elf && section && name, E_NULL);

  for (int i = 0; i < elf->header->e_shnum; i++) {
    if (strcmp(&elf->shstrtab[elf->sections[i].sh_name], name) == 0) {
      section->header = &elf->sections[i];
      section->data = elf->data + elf->sections[i].sh_offset;
      return E_OK;
    }
  }

  return E_NOTFOUND;
}

error_t elf_find_symbol(elf_t * elf, elf_symbol_t * symbol, const char * name) {
  // TODO: Lazy loading, if section is NULL, try to load?
  elf_section_t * sections[] = {&elf->symtab, &elf->dynsym};

  for (size_t i = 0; i < UTIL_ARR_SIZE(sections); ++i) {
    elf_section_t * section = sections[i];

    if (!section->header) {
      continue;
    }

    size_t symbol_count = section->header->sh_size / sizeof(Elf32_Sym);

    const char * strtab = (char *) elf->data + elf->sections[section->header->sh_link].sh_offset;

    for (size_t j = 0; j < symbol_count; ++j) {
      if (!strcmp(&strtab[section->sym[j].st_name], name)) {
        symbol->name = name;
        symbol->sym = &section->sym[j];
        symbol->addr = section->sym[j].st_value ? elf->data + section->sym[j].st_value : 0;
        return E_OK;
      }
    }
  }

  return E_NOTFOUND;
}

#if ELF_ENABLE_DUMP
error_t elf_dump(elf_t * elf) {
  ASSERT_RETURN(elf, E_NULL);

  elf_dump_general(elf);
  elf_dump_segments(elf);
  elf_dump_sections(elf);
  elf_dump_symbols(elf);
  elf_dump_unresolved_dynamic_symbols(elf);
  elf_dump_got(elf);

  return E_OK;
}

error_t elf_dump_general(elf_t * elf) {
  ASSERT_RETURN(elf, E_NULL);

  // TODO: Implement

  return E_OK;
}

error_t elf_dump_segments(elf_t * elf) {
  ASSERT_RETURN(elf, E_NULL);

  Elf32_Phdr * phdr = (Elf32_Phdr *) (elf->data + elf->header->e_phoff);

  log_info("Program Headers:");
  log_info("#  Type Offset VAddr  MemSize");

  for (int i = 0; i < elf->header->e_phnum; i++) {
    log_info("%02d %02u   0x%04x 0x%04x 0x%04x",
           i, phdr[i].p_type, phdr[i].p_offset, phdr[i].p_vaddr, phdr[i].p_memsz);
  }

  return E_OK;
}

error_t elf_dump_sections(elf_t * elf) {
  ASSERT_RETURN(elf, E_NULL);

  log_info("Sections:");

  log_info("%-2s %-16s %-2s %-6s %-6s", "#", "Name", "Type", "Offset", "Size");

  for (size_t i = 0; i < elf->header->e_shnum; ++i) {
    log_info(
      "%02d %-16s %02u   0x%04x  0x%04x",
       i,
       &elf->shstrtab[elf->sections[i].sh_name],
       elf->sections[i].sh_type,
       elf->sections[i].sh_offset,
       elf->sections[i].sh_size
    );
  }

  return E_OK;
}

error_t elf_dump_symbols(elf_t * elf) {
  ASSERT_RETURN(elf, E_NULL);

  elf_section_t * sections[] = {&elf->symtab, &elf->dynsym};

  for (size_t i = 0; i < UTIL_ARR_SIZE(sections); ++i) {
    elf_section_t * section = sections[i];

    if (!section->header) {
      continue;
    }

    size_t symbol_count = section->header->sh_size / sizeof(Elf32_Sym);

    const char * strtab = (char *) elf->data + elf->sections[section->header->sh_link].sh_offset;

    log_info("Symbols from section %s:", &elf->shstrtab[section->header->sh_name]);
    log_info("%-20s %-16s %-8s %-8s", "Name", "Value", "Size", "Type");

    for (size_t j = 0; j < symbol_count; ++j) {
      log_info(
        "%-20s %016x %-8u %-8s",
        &strtab[section->sym[j].st_name],
        section->sym[j].st_value, section->sym[j].st_size,
        elf_st_type_to_string(ELF32_ST_TYPE(section->sym[j].st_info))
      );
    }
  }

  return E_OK;
}

error_t elf_dump_unresolved_dynamic_symbols(elf_t * elf) {
  ASSERT_RETURN(elf, E_NULL);
  ASSERT_RETURN(elf->dynsym.header, E_NULL);
  ASSERT_RETURN(elf->dynstr.header, E_NULL);

  size_t dynsym_count = elf->dynsym.header->sh_size / sizeof(Elf32_Sym);
  Elf32_Sym * dynsym = elf->dynsym.sym;

  log_info("Unresolved Dynamic Symbols:");

  log_info("%-20s %-16s %-8s %-16s", "Name", "Value", "Size", "Type");

  for (size_t i = 0; i < dynsym_count; ++i) {
    if (ELF32_ST_BIND(dynsym[i].st_info) == STB_GLOBAL && dynsym[i].st_shndx == SHN_UNDEF) {
      log_info("%-20s %016x %-8u %-16s",
        null_str_guard(dynstr_get(elf, dynsym[i].st_name)),
        dynsym[i].st_value, dynsym[i].st_size,
        elf_st_type_to_string(ELF32_ST_TYPE(dynsym[i].st_info))
      );
    }
  }

  return E_OK;
}

error_t elf_dump_got(elf_t * elf) {
  ASSERT_RETURN(elf, E_NULL);
  ASSERT_RETURN(elf->got.header, E_NULL);

  Elf32_Addr * got_entries = elf->got.addr;
  size_t entries_count = elf->got.header->sh_size / sizeof(Elf32_Addr);

  log_info("GOT section at 0x%x (size=%u entries=%u), entries:",
    elf->got.header->sh_offset, elf->got.header->sh_size, entries_count);

  for (size_t i = 0; i < entries_count; i++) {
    Elf32_Addr got_entry = got_entries[i];

    Elf32_Sym * symbol = &elf->dynsym.sym[i];
    const char *symbol_name = dynstr_get(elf, symbol->st_name);

    log_info("GOT entry %u: 0x%08x (symbol: %s)",
      i, got_entry, null_str_guard(symbol_name));
  }

  return E_OK;
}
#endif

const char * elf_st_type_to_string(uint8_t type) {
#if ELF_CONVERT_TYPES_TO_STRING
  switch (type) {
    case STT_NOTYPE:  return "NOTYPE";
    case STT_OBJECT:  return "OBJECT";
    case STT_FUNC:    return "FUNC";
    case STT_SECTION: return "SECTION";
    case STT_FILE:    return "FILE";
    default:
      return "UNKNOWN";
  }
#else
  static char buf[4] = {0};

  sprintf(buf, "%d", type);

  return buf;
#endif
}

const char * elf_rel_type_to_string(uint8_t type) {
#if ELF_CONVERT_TYPES_TO_STRING
  switch (type) {
    case R_ARM_JUMP_SLOT: return "R_ARM_JUMP_SLOT";
    case R_ARM_GLOB_DAT:  return "R_ARM_GLOB_DAT";
    case R_ARM_ABS32:     return "R_ARM_ABS32";
    case R_ARM_RELATIVE:  return "R_ARM_RELATIVE";
    default:
      return "UNKNOWN";
  }
#else
  static char buf[4] = {0};

  sprintf(buf, "%d", type);

  return buf;
#endif
}

const char * elf_dyn_tag_to_string(uint32_t dyn_tag) {
#if ELF_CONVERT_TYPES_TO_STRING
  switch (dyn_tag) {
    case DT_NULL: return "DT_NULL";
    case DT_NEEDED: return "DT_NEEDED";
    case DT_PLTRELSZ: return "DT_PLTRELSZ";
    case DT_PLTGOT: return "DT_PLTGOT";
    case DT_HASH: return "DT_HASH";
    case DT_STRTAB: return "DT_STRTAB";
    case DT_SYMTAB: return "DT_SYMTAB";
    case DT_RELA: return "DT_RELA";
    case DT_RELASZ: return "DT_RELASZ";
    case DT_RELAENT: return "DT_RELAENT";
    case DT_STRSZ: return "DT_STRSZ";
    case DT_SYMENT: return "DT_SYMENT";
    case DT_INIT: return "DT_INIT";
    case DT_FINI: return "DT_FINI";
    case DT_SONAME: return "DT_SONAME";
    case DT_RPATH: return "DT_RPATH";
    case DT_SYMBOLIC: return "DT_SYMBOLIC";
    case DT_REL: return "DT_REL";
    case DT_RELSZ: return "DT_RELSZ";
    case DT_RELENT: return "DT_RELENT";
    case DT_PLTREL: return "DT_PLTREL";
    case DT_DEBUG: return "DT_DEBUG";
    case DT_TEXTREL: return "DT_TEXTREL";
    case DT_JMPREL: return "DT_JMPREL";
    case DT_BIND_NOW: return "DT_BIND_NOW";
    case DT_INIT_ARRAY: return "DT_INIT_ARRAY";
    case DT_FINI_ARRAY: return "DT_FINI_ARRAY";
    case DT_INIT_ARRAYSZ: return "DT_INIT_ARRAYSZ";
    case DT_FINI_ARRAYSZ: return "DT_FINI_ARRAYSZ";
    case DT_RUNPATH: return "DT_RUNPATH";
    case DT_FLAGS: return "DT_FLAGS";
    case DT_ENCODING: return "DT_ENCODING";
    case DT_PREINIT_ARRAYSZ: return "DT_PREINIT_ARRAYSZ";
    case DT_NUM: return "DT_NUM";
    case DT_HIPROC: return "DT_HIPROC";
    case DT_PROCNUM: return "DT_PROCNUM";
    case DT_GNU_PRELINKED: return "DT_GNU_PRELINKED";
    case DT_GNU_CONFLICTSZ: return "DT_GNU_CONFLICTSZ";
    case DT_GNU_LIBLISTSZ: return "DT_GNU_LIBLISTSZ";
    case DT_CHECKSUM: return "DT_CHECKSUM";
    case DT_PLTPADSZ: return "DT_PLTPADSZ";
    case DT_MOVEENT: return "DT_MOVEENT";
    case DT_MOVESZ: return "DT_MOVESZ";
    case DT_FEATURE_1: return "DT_FEATURE_1";
    case DT_POSFLAG_1: return "DT_POSFLAG_1";
    case DT_SYMINSZ: return "DT_SYMINSZ";
    default:
      return "UNKNOWN";
  }
#else
  static char buf[8] = {0};

  sprintf(buf, "%d", type);

  return buf;
#endif
}
