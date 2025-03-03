/** ========================================================================= *
 *
 * @file timeout.h
 * @date 20-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "time/time.h"
#include <stdbool.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Creates timeout variable and starts the timeout with specified ms value
 *
 * @param name Timeout variable name
 * @param ms   Timeout value (ms)
 */
#define TIMEOUT_CREATE(name, ms)  \
    timeout_t name;               \
    timeout_start(&name, ms)

/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Timeout context, holds start time and duration
 * Start time is set in timeout_start()
 * Uses runtime_get()
 */
typedef struct {
  milliseconds_t start;
  milliseconds_t duration;
} timeout_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Starts timeout
 */
void timeout_start(timeout_t * timeout, milliseconds_t ms);

/**
 * Restarts timeout with current duration
 * @note Won't work, if timeout was manually expired
 */
void timeout_restart(timeout_t * timeout);

/**
 * Checks if timeout is expired
 */
bool timeout_is_expired(const timeout_t * timeout);

/**
 * Expires timeout
 */
void timeout_expire(timeout_t * timeout);

#ifdef __cplusplus
}
#endif

