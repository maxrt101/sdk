/** ========================================================================= *
 *
 * @file queue.c
 * @date 10-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Simple ring buffer-like queue implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "queue/queue.h"
#include "error/assertion.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
error_t queue_init(queue_t * q, queue_element_t * elements, size_t capacity) {
  ASSERT_RETURN(q && elements, E_NULL);

  q->elements = elements;
  q->capacity = capacity;
  q->size = 0;
  q->head = 0;
  q->tail = 0;

  return E_OK;
}

error_t queue_clear(queue_t * q) {
  ASSERT_RETURN(q, E_NULL);

  q->size = 0;
  q->head = 0;
  q->tail = 0;

  return E_OK;
}

size_t queue_capacity(queue_t * q) {
  return q ? q->capacity : 0;
}

size_t queue_size(queue_t * q) {
  return q ? q->size : 0;
}

error_t queue_push(queue_t * q, queue_element_t data) {
  ASSERT_RETURN(q, E_NULL);

  q->elements[q->head] = data;

  size_t next_head = q->head;

  if (q->head + 1 == q->capacity) {
    next_head = 0;
  } else {
    next_head += 1;
  }

  if (next_head == q->tail) {
    return E_OVERFLOW;
  }

  q->head = next_head;
  q->size += 1;

  return E_OK;
}

error_t queue_pop(queue_t * q, queue_element_t * data) {
  ASSERT_RETURN(q && data, E_NULL);

  if (q->head == q->tail) {
    return E_UNDERFLOW;
  }

  *data = q->elements[q->tail];

  if (q->tail + 1 == q->capacity) {
    q->tail = 0;
  } else {
    q->tail += 1;
  }

  q->size -= 1;

  return E_OK;
}

error_t queue_peek(queue_t * q, queue_element_t * data) {
  ASSERT_RETURN(q && data, E_NULL);

  if (q->head == q->tail) {
    return E_EMPTY;
  }

  *data = q->elements[q->tail];

  return E_OK;
}