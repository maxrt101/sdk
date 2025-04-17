/** ========================================================================= *
 *
 * @file alloc.h
 * @date 27-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Allocator. os_alloc = malloc, os_free = free. To use, a heap must
 *        be created with a buffer
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "os/heap/heap.h"
#include "error/error.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Sets internal OS heap
 *
 * @param heap Heap Context to use
 */
error_t os_use_heap(os_heap_t * heap);

/**
 * Returns internal OS heap
 */
os_heap_t * os_get_heap(void);

/**
 * Allocates memory from internal OS heap
 *
 * @param size Size of allocation
 */
void * os_alloc(size_t size);

/**
 * Returns memory to internal OS heap
 *
 * @param ptr Memory that was previously allocated
 */
error_t os_free(void * ptr);

/**
 * Defragments the os_heap
 */
error_t os_defrag(void);

#ifdef __cplusplus
}
#endif
