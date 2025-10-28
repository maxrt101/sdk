/** ========================================================================= *
 *
 * @file event.c
 * @date 27-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "os/event.h"
#include "error/assertion.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG os

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
static void os_event_unblock_all(os_event_t * event) {
  for (size_t i = 0; i < OS_EVENT_MAX_SUBSCRIBERS; ++i) {
    if (event->subscribers[i]) {
      OS_LOG_TRACE(EVENT, "os_event: notify '%s' on '%s'",
        event->subscribers[i]->name, event->name);

      event->subscribers[i]->state = OS_TASK_STATE_READY;
    }
  }
}

/* Shared functions ========================================================= */
error_t os_event_init(os_event_t * event, const char * name) {
  ASSERT_RETURN(event, E_NULL);

  event->name = name;
  memset(event->subscribers, 0, sizeof(event->subscribers));

  OS_LOG_TRACE(EVENT, "os_event_init: '%s'", event->name);

  return E_OK;
}

error_t os_event_reset(os_event_t * event) {
  ASSERT_RETURN(event, E_NULL);

  OS_LOG_TRACE(EVENT, "os_event_reset: '%s'", event->name);

  os_event_unblock_all(event);
  memset(event->subscribers, 0, sizeof(event->subscribers));

  return E_OK;
}

error_t os_event_subscribe(os_event_t * event) {
  ASSERT_RETURN(event, E_NULL);

  OS_LOG_TRACE(EVENT, "os_event: subscribe '%s' to '%s'",
    os_task_current()->name, event->name);

  for (size_t i = 0; i < OS_EVENT_MAX_SUBSCRIBERS; ++i) {
    if (!event->subscribers[i]) {
      event->subscribers[i] = os_task_current();
      return E_OK;
    }
  }

  log_error("os_event: failed to subscribe '%s' to '%s'",
    os_task_current()->name, event->name);

  return E_OVERFLOW;
}

error_t os_event_unsubscribe(os_event_t * event) {
  ASSERT_RETURN(event, E_NULL);

  OS_LOG_TRACE(EVENT, "os_event: unsubscribe '%s' from '%s'",
    os_task_current()->name, event->name);

  for (size_t i = 0; i < OS_EVENT_MAX_SUBSCRIBERS; ++i) {
    if (event->subscribers[i] == os_task_current()) {
      event->subscribers[i] = NULL;
      return E_OK;
    }
  }

  log_error("os_event: failed to unsubscribe '%s' from '%s'",
    os_task_current()->name, event->name);

  return E_NOTFOUND;
}

error_t os_event_trigger(os_event_t * event) {
  ASSERT_RETURN(event, E_NULL);

  OS_LOG_TRACE(EVENT, "os_event: trigger '%s'", event->name);

  os_event_unblock_all(event);

  return E_OK;
}

error_t os_event_wait(os_event_t * event) {
  ASSERT_RETURN(event, E_NULL);

  bool is_current_task_subscribed = false;

  for (size_t i = 0; i < OS_EVENT_MAX_SUBSCRIBERS; ++i) {
    if (event->subscribers[i] == os_task_current()) {
      is_current_task_subscribed = true;
      break;
    }
  }

  if (!is_current_task_subscribed) {
    log_error("Can't lock '%s' on '%s'. Not subscribed",
      os_task_current()->name, event->name);
    return E_NOTFOUND;
  }

  OS_LOG_TRACE(EVENT, "os_event: locking '%s' on '%s'",
    os_task_current()->name, event->name);

  os_task_current()->state = OS_TASK_STATE_LOCKED;
  os_schedule();

  return E_OK;
}
