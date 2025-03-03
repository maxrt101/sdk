/** ========================================================================= *
 *
 * @file alloc.c
 * @date 27-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "os/alloc/alloc.h"
#include "error/assertion.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
static os_heap_t * os_heap;

/* Private functions ======================================================== */
/* Shared functions ========================================================= */
error_t os_use_heap(os_heap_t * heap) {
  ASSERT_RETURN(heap, E_NULL);
  os_heap = heap;
  return E_OK;
}

void * os_alloc(size_t size) {
  return os_heap_alloc(os_heap, size);
}

error_t os_free(void * ptr) {
  return os_heap_free(os_heap, ptr);
}

error_t os_defrag(void) {
  return os_heap_defrag(os_heap);
}
