/** ========================================================================= *
 *
 * @file event.h
 * @date 27-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "os/os.h"

/* Defines ================================================================== */
/**
 * Size of subscribers list in event
 */
#ifndef OS_EVENT_MAX_SUBSCRIBERS
#define OS_EVENT_MAX_SUBSCRIBERS 4
#endif

/**
 * If enabled, will trace all event operations to log_debug
 */
#ifndef USE_OS_TRACE_EVENT
#define USE_OS_TRACE_EVENT 0
#endif


/* Macros =================================================================== */
/**
 * Creates an event
 */
#define OS_CREATE_EVENT(__name)     \
  os_event_t __name = {             \
    .name = UTIL_STRINGIFY(__name), \
    .subscribers = {0},             \
  };

/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * OS Event context
 */
typedef struct {
  const char * name;
  os_task_t *  subscribers[OS_EVENT_MAX_SUBSCRIBERS];
} os_event_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Initialize an event context
 *
 * @param event Event handle
 * @param name Event name
 */
error_t os_event_init(os_event_t * event, const char * name);

/**
 * Reset event. Will notify all subscribers & reset subscriber list
 *
 * @param event Event handle
 */
error_t os_event_reset(os_event_t * event);

/**
 * Subscribe to event
 *
 * @note Works only in task context. Will subscribe current task
 *
 */
error_t os_event_subscribe(os_event_t * event);

/**
 * Unsubscribe from event
 *
 * @note Works only in task context. Will unsubscribe current task
 *
 * @param event Event handle
 */
error_t os_event_unsubscribe(os_event_t * event);

/**
 * Trigger this event. Will notify all subscribers
 *
 * @param event Event handle
 */
error_t os_event_trigger(os_event_t * event);

/**
 * Block current task until event is triggered
 *
 * @param event Event handle
 */
error_t os_event_wait(os_event_t * event);

#ifdef __cplusplus
}
#endif
