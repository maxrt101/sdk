#include <loader.h>

#if WITH_LOADER_MACHO_CONTAINER

#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/reloc.h>

#include <string.h>

#include <log/optional.h>

LOGGER_USE(loader);

/**
 * @brief Utility to parse Unsigned Little Endian Base 128 (used in Mach-O opcodes)
 */
__STATIC_INLINE uint64_t macho_read_uleb128(uint8_t ** p) {
  uint64_t result = 0;
  int bit = 0;

  do {
    result |= (uint64_t)(**p & 0x7f) << bit; bit += 7;
  } while (*(*p)++ & 0x80);

  return result;
}

/**
 * @brief Utility to parse Signed Little Endian Base 128 (used in Mach-O opcodes)
 */
__STATIC_INLINE int64_t macho_read_sleb128(uint8_t ** p) {
  int64_t result = 0;
  int bit = 0;
  uint8_t byte;

  do {
    byte = *(*p)++; result |= ((int64_t)(byte & 0x7f)) << bit; bit += 7;
  } while (byte & 0x80);

  if ((byte & 0x40) && (bit < 64)) {
    result |= (~0ULL) << bit;
  }

  return result;
}

/**
 * @brief Translate a segment index from the opcode into a raw file offset
 */
static uint64_t macho_get_segment_fileoff(loader_file_t * file, uint8_t seg_index) {
  struct mach_header_64 * header = (struct mach_header_64 *) file->data;
  struct load_command * lc = (struct load_command *) ((uint8_t *) file->data + sizeof(struct mach_header_64));
  uint8_t current_seg = 0;

  for (uint32_t i = 0; i < header->ncmds; i++) {
    if (lc->cmd == LC_SEGMENT_64) {
      if (current_seg == seg_index) {
        return ((struct segment_command_64 *)lc)->fileoff;
      }

      current_seg++;
    }

    lc = (struct load_command *) ((uint8_t *) lc + lc->cmdsize);
  }

  return 0;
}

/**
 * @brief Convert a Virtual Address (vmaddr) into a flat file offset
 */
static uint64_t macho_vmaddr_to_fileoff(loader_file_t * file, uint64_t vmaddr) {
  struct mach_header_64 * header = (struct mach_header_64 *) file->data;
  struct load_command * lc = (struct load_command *) ((uint8_t *) file->data + sizeof(struct mach_header_64));

  for (uint32_t i = 0; i < header->ncmds; i++) {
    if (lc->cmd == LC_SEGMENT_64) {
      struct segment_command_64 * seg = (struct segment_command_64 *)lc;

      if (vmaddr >= seg->vmaddr && vmaddr < seg->vmaddr + seg->vmsize) {
        return seg->fileoff + (vmaddr - seg->vmaddr);
      }
    }

    lc = (struct load_command *) ((uint8_t *) lc + lc->cmdsize);
  }

  return vmaddr;
}

static error_t macho_process_rebase_opcodes(loader_file_t * file, uint8_t * start, uint32_t size) {
  if (!start || size == 0) return E_OK;
  uint8_t * p = start;
  uint8_t * end = p + size;
  uint64_t segment_offset = 0;

  while (p < end) {
    uint8_t opcode = *p & REBASE_OPCODE_MASK;
    uint8_t immediate = *p & REBASE_IMMEDIATE_MASK;
    p++;

    switch (opcode) {
      case REBASE_OPCODE_DONE: {
        return E_OK;
      }

      case REBASE_OPCODE_SET_TYPE_IMM: {
        break;
      }

      case REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB: {
        segment_offset = macho_get_segment_fileoff(file, immediate) + macho_read_uleb128(&p);
        break;
      }

      case REBASE_OPCODE_ADD_ADDR_ULEB: {
        segment_offset += macho_read_uleb128(&p);
        break;
      }

      case REBASE_OPCODE_ADD_ADDR_IMM_SCALED: {
        segment_offset += immediate * sizeof(void *);
        break;
      }

      case REBASE_OPCODE_DO_REBASE_IMM_TIMES: {
        for (int i = 0; i < immediate; i++) {
          uint64_t * patch_addr = (uint64_t *)((uint8_t *)file->data + segment_offset);
          *patch_addr = (uint64_t)file->data + macho_vmaddr_to_fileoff(file, *patch_addr);
          segment_offset += sizeof(void *);
        }

        break;
      }

      case REBASE_OPCODE_DO_REBASE_ULEB_TIMES: {
        uint64_t count = macho_read_uleb128(&p);

        for (uint64_t i = 0; i < count; i++) {
          uint64_t * patch_addr = (uint64_t *)((uint8_t *)file->data + segment_offset);
          *patch_addr = (uint64_t)file->data + macho_vmaddr_to_fileoff(file, *patch_addr);
          segment_offset += sizeof(void *);
        }

        break;
      }

      case REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB: {
        uint64_t * patch_addr = (uint64_t *)((uint8_t *)file->data + segment_offset);
        *patch_addr = (uint64_t)file->data + macho_vmaddr_to_fileoff(file, *patch_addr);
        segment_offset += sizeof(void *) + macho_read_uleb128(&p);
        break;
      }

      case REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB: {
        uint64_t count = macho_read_uleb128(&p);
        uint64_t skip = macho_read_uleb128(&p);

        for (uint64_t i = 0; i < count; i++) {
          uint64_t * patch_addr = (uint64_t *)((uint8_t *)file->data + segment_offset);
          *patch_addr = (uint64_t)file->data + macho_vmaddr_to_fileoff(file, *patch_addr);
          segment_offset += sizeof(void *) + skip;
        }

        break;
      }

      default: {
        return E_NOTIMPL;
      }

    }
  }

  return E_OK;
}

static error_t macho_process_bind_opcodes(loader_file_t * file, uint8_t * start, uint32_t size) {
  ASSERT_RETURN(start && size, E_OK);

  uint8_t *    p = start;
  uint8_t *    end = p + size;
  const char * symbol_name = NULL;
  uint64_t     segment_offset = 0;

  while (p < end) {
    uint8_t opcode = *p & BIND_OPCODE_MASK;
    uint8_t immediate = *p & BIND_IMMEDIATE_MASK;
    p++;

    switch (opcode) {
      case BIND_OPCODE_DONE: {
        break;
      }

      case BIND_OPCODE_SET_DYLIB_ORDINAL_IMM:
      case BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB: {
        if (opcode == BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB) {
          macho_read_uleb128(&p);
        }
        break;
      }

      case BIND_OPCODE_SET_DYLIB_SPECIAL_IMM:
      case BIND_OPCODE_SET_TYPE_IMM: {
        break;
      }

      case BIND_OPCODE_SET_ADDEND_SLEB: {
        macho_read_sleb128(&p);
        break;
      }

      case BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM: {
        symbol_name = (const char *) p;
        p += strlen(symbol_name) + 1;
        break;
      }

      case BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB: {
        segment_offset = macho_get_segment_fileoff(file, immediate) + macho_read_uleb128(&p);
        break;
      }

      case BIND_OPCODE_ADD_ADDR_ULEB: {
        segment_offset += macho_read_uleb128(&p);
        break;
      }

      case BIND_OPCODE_DO_BIND:
      case BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB:
      case BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED: {
        if (symbol_name) {
          const char * clean_name = (symbol_name[0] == '_') ? symbol_name + 1 : symbol_name;
          if (strcmp(clean_name, "dyld_stub_binder") != 0) {
            uint64_t resolved_addr = (uint64_t) file->resolver(clean_name);
            ASSERT_RETURN(resolved_addr, E_NOTFOUND);

            uint64_t * patch_addr = (uint64_t *) ((uint8_t *) file->data + segment_offset);
            *patch_addr = resolved_addr;
          }
        }

        segment_offset += sizeof(void *);

        if (opcode == BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB) {
          segment_offset += macho_read_uleb128(&p);
        } else if (opcode == BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED) {
          segment_offset += immediate * sizeof(void *);
        }

        break;
      }

      case BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB: {
        uint64_t count = macho_read_uleb128(&p);
        uint64_t skip = macho_read_uleb128(&p);

        for (uint64_t i = 0; i < count; i++) {
          if (symbol_name) {
            const char * clean_name = (symbol_name[0] == '_') ? symbol_name + 1 : symbol_name;

            if (strcmp(clean_name, "dyld_stub_binder") != 0) {
              uint64_t resolved_addr = (uint64_t) file->resolver(clean_name);
              ASSERT_RETURN(resolved_addr, E_NOTFOUND);

              uint64_t * patch_addr = (uint64_t *) ((uint8_t *) file->data + segment_offset);
              *patch_addr = resolved_addr;
            }
          }

          segment_offset += sizeof(void *) + skip;
        }

        break;
      }

      default: {
        return E_NOTIMPL;
      }
    }
  }

  return E_OK;
}

static bool macho_probe(loader_file_t * file) {
  if (file->size < sizeof(uint32_t)) return false;
  uint32_t magic = *(uint32_t *) file->data;
  return (magic == MH_MAGIC_64 || magic == MH_CIGAM_64);
}

static error_t macho_load(loader_file_t * file) {
  struct mach_header_64 * header = (struct mach_header_64 *) file->data;
  struct load_command * lc = (struct load_command *) ((uint8_t *) file->data + sizeof(struct mach_header_64));
  struct dyld_info_command * dyld_info = NULL;

  for (uint32_t i = 0; i < header->ncmds; i++) {
    if (lc->cmd == LC_DYLD_INFO || lc->cmd == LC_DYLD_INFO_ONLY) {
      dyld_info = (struct dyld_info_command *) lc;
      break;
    }

    lc = (struct load_command *) ((uint8_t *) lc + lc->cmdsize);
  }

  ASSERT_RETURN(dyld_info, E_OK);

  // Process Rebasing (R_ARM_RELATIVE equivalent)
  ERROR_CHECK_RETURN(macho_process_rebase_opcodes(file, (uint8_t *)file->data + dyld_info->rebase_off, dyld_info->rebase_size));

  // Process Bindings (Global variables, data pointers)
  ERROR_CHECK_RETURN(macho_process_bind_opcodes(file, (uint8_t *)file->data + dyld_info->bind_off, dyld_info->bind_size));

  // Process Lazy Bindings (Function calls)
  ERROR_CHECK_RETURN(macho_process_bind_opcodes(file, (uint8_t *)file->data + dyld_info->lazy_bind_off, dyld_info->lazy_bind_size));

  return E_OK;
}

static void * macho_sym_lookup(loader_file_t * file, const char * name) {
  struct mach_header_64 * header = (struct mach_header_64 *) file->data;
  struct load_command * lc = (struct load_command *) ((uint8_t *) file->data + sizeof(struct mach_header_64));
  struct symtab_command * symtab = NULL;

  for (uint32_t i = 0; i < header->ncmds; i++) {
    if (lc->cmd == LC_SYMTAB) {
      symtab = (struct symtab_command *) lc;
      break;
    }

    lc = (struct load_command *) ((uint8_t *) lc + lc->cmdsize);
  }

  ASSERT_RETURN(symtab, NULL);

  struct nlist_64 * syms = (struct nlist_64 *) ((uint8_t *) file->data + symtab->symoff);
  const char * strs = (const char *) ((uint8_t *) file->data + symtab->stroff);

  for (uint32_t i = 0; i < symtab->nsyms; i++) {
    uint32_t strx = syms[i].n_un.n_strx;

    if (strx > 0) {
      const char * sym_name = strs + strx;

      if (sym_name[0] == '_') {
        sym_name++;
      }

      if (!strcmp(sym_name, name) && (syms[i].n_type & N_TYPE) != N_UNDF) {
        // n_value is a Virtual Address. Translate it to our flat memory map offset.
        uint64_t fileoff = macho_vmaddr_to_fileoff(file, syms[i].n_value);
        return (void *) ((uint8_t *) file->data + fileoff);
      }
    }
  }

  return NULL;
}

LOADER_DECLARE(macho, {
  .probe      = macho_probe,
  .load       = macho_load,
  .sym_lookup = macho_sym_lookup,
});

#endif
