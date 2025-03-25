/** ========================================================================= *
 *
 * @file heap.h
 * @date 27-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Heap Implementation
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stddef.h>
#include <stdint.h>
#include "error/error.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Heap Block Context
 */
typedef struct os_heap_block_s {
  struct os_heap_block_s * next; /** Next Block */
  size_t size;                   /** Size of current block */
  enum {
    OS_HEAP_BLOCK_FREE = 0,
    OS_HEAP_BLOCK_USED
  } state;                       /** State of current block (FREE/USED) */
  uint8_t data[0];
} os_heap_block_t;

/**
 * Heap Context
 */
typedef struct {
  /** Root Block */
  os_heap_block_t * root_block;

  /** Heap metadata */
  size_t size;
  uint8_t * start;
} os_heap_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes a heap
 *
 * @param heap Heap Context
 * @param start Start of memory block
 * @param size Size of memory block
 */
error_t os_heap_create(os_heap_t * heap, void * start, size_t size);

/**
 * Destroys a heap
 *
 * @param heap Heap Context
 */
error_t os_heap_destroy(os_heap_t * heap);

/**
 * Erases a heap
 * Resets all allocations
 *
 * @param heap Heap Context
 */
error_t os_heap_erase(os_heap_t * heap);

/**
 * Allocate a block of memory
 *
 * @param heap Heap Context
 * @param size Size of block to allocate
 * @retval Pointer to a block of memory of specified size, or NULL,
 *         if allocation failed
 */
void * os_heap_alloc(os_heap_t * heap, size_t size);

/**
 * Return allocated block of memory to the heap
 *
 * @param heap Heap Context
 * @param ptr Previously allocated pointer
 */
error_t os_heap_free(os_heap_t * heap, void * ptr);

/**
 * Defragment the heap
 *
 * @note Heaps gets fragmented on every allocation, so defrag operation must be
 *       performed to merge small free blocks.
 *
 * @note Use OS_HEAP_DEFRAG_ON_FREE to perform defrag after free
 *       (free may get slower)
 *
 * @param heap Heap Context
 * @param ptr Previously allocated pointer
 */
error_t os_heap_defrag(os_heap_t * heap);

#ifdef __cplusplus
}
#endif
