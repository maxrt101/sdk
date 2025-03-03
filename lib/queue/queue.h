/** ========================================================================= *
 *
 * @file queue.h
 * @date 10-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Simple ring buffer-like queue implementation
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/compiler.h"
#include <stdbool.h>
#include <stddef.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Defines a queue
 *
 * @note Actually consists of 2 statements - declaration of buffer and queue
 * @note queue_init can be used instead for explicit initialization
 *
 * @param name Queue name (variable)
 * @param cap Capacity of queue
 */
#define QUEUE_DEFINE(name, cap)                             \
    queue_element_t UTIL_CAT(name, _queue_buf)[cap] = {0};  \
    queue_t name = {                                        \
        .elements = UTIL_CAT(name, _queue_buf),             \
        .capacity = cap,                                    \
        .size = 0,                                          \
        .head = 0,                                          \
        .tail = 0                                           \
    }

/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Queue element
 */
typedef void * queue_element_t;

/**
 * Queue context
 */
typedef struct {
  queue_element_t * elements; /** Buffer for elements */
  size_t capacity;            /** Capacity of buffer */
  size_t size;                /** Size of queue */
  size_t head;                /** Index of queue head */
  size_t tail;                /** Index of queue tail */
} queue_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initializes queue
 *
 * @note QUEUE_DEFINE can be used instead for automatic buffer creation
 *       and queue initialization
 *
 * @param q Queue handle
 * @param elements Queue buffer
 * @param size Buffer size
 */
error_t queue_init(queue_t * q, queue_element_t * elements, size_t capacity);

/**
 * Resets the queue (sets head & tail to 0)
 *
 * @param q Queue handle
 */
error_t queue_clear(queue_t * q);

/**
 * Returns queue capacity
 *
 * @param q Queue handle
 */
size_t queue_capacity(queue_t * q);

/**
 * Returns queue size
 *
 * @param q Queue handle
 */
size_t queue_size(queue_t * q);

/**
 * Push element into the queue (sets data to buf[head] and increases head)
 *
 * @param q Queue handle
 * @param data Element to push
 */
error_t queue_push(queue_t * q, queue_element_t data);

/**
 * Pop element from the queue (reads data from buf[tail] and increases tail)
 *
 * @param q Queue handle
 * @param data Element to pop tail into
 */
error_t queue_pop(queue_t * q, queue_element_t * data);

/**
 * Read element at buf[tail] but don't pop it
 *
 * @param q Queue handle
 * @param data Element to copy tail into
 */
error_t queue_peek(queue_t * q, queue_element_t * data);

#ifdef __cplusplus
}
#endif
