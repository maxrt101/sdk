/** ========================================================================= *
 *
 * @file elf.h
 * @date 13-03-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief ELF Support & units
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include <stddef.h>
#include <stdint.h>
#include <elf.h>

/* Defines ================================================================== */
/**
 * If enabled - will convert section/tag/etc types to string
 */
#ifndef ELF_CONVERT_TYPES_TO_STRING
#define ELF_CONVERT_TYPES_TO_STRING 1
#endif

/**
 * Enables compilation of elf_dump* functions
 */
#ifndef ELF_ENABLE_DUMP
#define ELF_ENABLE_DUMP 1
#endif


/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Metadata for ELF section
 */
typedef struct {
  Elf32_Shdr * header;
  const char * name;
  union {
    uint8_t *    data;
    const char * str;
    Elf32_Dyn *  dyn;
    Elf32_Sym *  sym;
    Elf32_Addr * addr;
  };
} elf_section_t;

/**
 * Metadata for ELF symbol
 */
typedef struct {
  const char * name;
  Elf32_Sym *  sym;
  void *       addr;
} elf_symbol_t;

/**
 * Metadata for ELF File
 */
typedef struct {
  /** Union of raw data and ELF header (which overlap) */
  union {
    uint8_t * data;
    Elf32_Ehdr * header;
  };

  /** Section headers */
  Elf32_Shdr * sections;

  /** String table */
  const char * shstrtab;

  /** Various sections for convenience */
  elf_section_t symtab;
  elf_section_t strtab;
  elf_section_t dynstr;
  elf_section_t dynsym;
  elf_section_t dynamic;
  elf_section_t got;
} elf_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Parses ELF File
 *
 * Checks validity, loads header, loads strtab and some sections
 *
 * @param elf ELF File context
 * @param data Pointer to raw ELF data
 */
error_t elf_parse(elf_t * elf, uint8_t * data);

/**
 * Checks if data pointed to by `data` is a valid ELF file
 *
 * @note Only checks header magic
 *
 * @param data Pointer to ELF data
 */
error_t elf_check(uint8_t * data);

/**
 * Retrieves section from ELF file
 *
 * @param elf ELF File context
 * @param section Where to put retrieved section
 * @param name Section name
 */
error_t elf_find_section(elf_t * elf, elf_section_t * section, const char * name);

/**
 * Retrieves symbol from ELF file
 *
 * @note Will look in .symtab & .dynsym sections
 *
 * @param elf ELF File context
 * @param symbol Where to put retrieved symbol
 * @param name Symbol name
 */
error_t elf_find_symbol(elf_t * elf, elf_symbol_t * symbol, const char * name);

// error_t elf_find_sections(elf_t * elf, elf_section_t * sections, size_t size);
// error_t elf_find_symbols(elf_t * elf, elf_symbol_t * symbols, size_t size);

#if ELF_ENABLE_DUMP
/**
 * Dumps all parsed info into console
 *
 * Equivalent to calling all elf_dump_* functions in a row
 *
 * @param elf ELF File context
 */
error_t elf_dump(elf_t * elf);

/**
 * Dumps general info (mostly from elf header) into console
 *
 * @param elf ELF File context
 */
error_t elf_dump_general(elf_t * elf);

/**
 * Dumps segments (program headers) into console
 *
 * @param elf ELF File context
 */
error_t elf_dump_segments(elf_t * elf);

/**
 * Dumps sections into console
 *
 * @param elf ELF File context
 */
error_t elf_dump_sections(elf_t * elf);

/**
 * Dumps symbols into console
 *
 * @param elf ELF File context
 */
error_t elf_dump_symbols(elf_t * elf);

/**
 * Dump unresolved dynamic sections into console
 *
 * @param elf ELF File context
 */
error_t elf_dump_unresolved_dynamic_symbols(elf_t * elf);

/**
 * Dump GOT (Global Offset Table) into console
 *
 * @param elf ELF File context
 */
error_t elf_dump_got(elf_t * elf);
#endif

/**
 * Converts symbol st_type to strings
 *
 * @param type Value of ELF32_ST_TYPE(Elf32_Sym::st_info)
 */
const char * elf_st_type_to_string(uint8_t type);

/**
 * Converts relocation type to string
 *
 * @param type Value of ELF32_R_TYPE(Elf32_Rel::r_info)
 */
const char * elf_rel_type_to_string(uint8_t type);

/**
 * Converts dynamic tag to string
 *
 * @param dyn_tag Value of Elf32_Dyn::d_tag
 */
const char * elf_dyn_tag_to_string(uint32_t dyn_tag);

#ifdef __cplusplus
}
#endif