/** ========================================================================= *
 *
 * @file heap.c
 * @date 27-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Heap Implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "os/heap/heap.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG HEAP

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
__STATIC_INLINE void * os_heap_alloc_impl(os_heap_block_t * block, size_t size) {
  if (!block) {
    log_error("os_heap_alloc: bad block");
    return NULL;
  }

  if (block->state == OS_HEAP_BLOCK_USED) {
    if (block->next) {
      return os_heap_alloc_impl(block->next, size);
    }
  } else {
    if (block->size >= size) {
      os_heap_block_t * new_block =
          (os_heap_block_t *) (block + block->size - size - sizeof(os_heap_block_t));

      new_block->next = NULL;
      new_block->size = size;
      new_block->state = OS_HEAP_BLOCK_USED;

      if (block->next) {
        new_block->next = block->next;
      }

      block->next = new_block;
      block->size -= size + sizeof(os_heap_block_t);

      uint8_t * result = (uint8_t *) new_block + sizeof(os_heap_block_t);

      log_debug("os_heap_alloc(%p, %u): new block=%p result=%p",
                block, size, new_block, result);

      return (void *) result;
    } else {
      if (block->next) {
        return os_heap_alloc_impl(block->next, size);
      }
    }
  }

  log_debug("os_heap_alloc: no memory left");
  return NULL;
}

__STATIC_INLINE error_t os_heap_free_impl(os_heap_block_t * block, void * ptr) {
  if (!block) {
    log_error("os_heap_free: bad block");
    return E_INVAL;
  }

  if (ptr == (uint8_t *) block + sizeof(os_heap_block_t)) {
    log_debug("os_heap_free(%p): block=%p size=%u", ptr, block, block->size);
    block->state = OS_HEAP_BLOCK_FREE;
    return E_OK;
  }

  if (block->next) {
    return os_heap_free_impl(block->next, ptr);
  }

  log_error("os_heap_free(%p): block not found", ptr);
  return E_NOTFOUND;
}

__STATIC_INLINE error_t os_heap_defrag_impl(os_heap_block_t * block) {
  if (!block) {
    return E_OK;
  }

  if (block->state != OS_HEAP_BLOCK_USED) {
    if (block->next && block->next->state != OS_HEAP_BLOCK_USED) {
      log_debug("os_heap_defrag: merging %p(size=%u) and %p(size=%u)",
                block, block->size, block->next, block->next->size);
      block->next = block->next->next;
      block->size += block->next->size;
      return os_heap_defrag_impl(block);
    }
  }

  return os_heap_defrag_impl(block->next);
}

/* Shared functions ========================================================= */
error_t os_heap_create(os_heap_t * heap, void * start, size_t size) {
  ASSERT_RETURN(heap, E_NULL);
  ASSERT_RETURN(start && size, E_INVAL);

  heap->size = size;
  heap->start = start;

  heap->root_block = (os_heap_block_t *) start;

  heap->root_block->next = NULL;
  heap->root_block->size = size;
  heap->root_block->state = OS_HEAP_BLOCK_FREE;

  log_debug("os_heap_create[%p]: %p %u", heap, start, size);

  return E_OK;
}

error_t os_heap_destroy(os_heap_t * heap) {
  ASSERT_RETURN(heap, E_NULL);

  memset(heap, 0, sizeof(os_heap_t));

  return E_OK;
}

error_t os_heap_erase(os_heap_t * heap) {
  ASSERT_RETURN(heap, E_NULL);

  memset(heap->root_block, 0, sizeof(os_heap_block_t));

  heap->root_block->next = NULL;
  heap->root_block->size = heap->size;
  heap->root_block->state = OS_HEAP_BLOCK_FREE;

  return E_OK;
}

void * os_heap_alloc(os_heap_t * heap, size_t size) {
  ASSERT_RETURN(heap, NULL);

  void * result = os_heap_alloc_impl(heap->root_block, size);

#if USE_OS_HEAP_DEFRAG_ON_NO_MEM
  if (!result) {
    os_heap_defrag(heap);
    result = os_heap_alloc_impl(heap->root_block, size);
  }
#endif

  return result;
}

error_t os_heap_free(os_heap_t * heap, void * ptr) {
  ASSERT_RETURN(heap, E_NULL);
  ASSERT_RETURN(ptr, E_INVAL);

  error_t err = os_heap_free_impl(heap->root_block, ptr);

#if USE_OS_HEAP_DEFRAG_ON_FREE
  if (err == E_OK) {
    err = os_heap_defrag(heap);
  }
#endif

  return err;
}

error_t os_heap_defrag(os_heap_t * heap) {
  ASSERT_RETURN(heap, E_NULL);
  return os_heap_defrag_impl(heap->root_block);
}
