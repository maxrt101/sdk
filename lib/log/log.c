/** ========================================================================= *
 *
 * @file log.c
 * @date 23-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "log/log.h"
#include "log/color.h"
#include "atomic/atomic.h"
#include <string.h>
#include <stdio.h>

/* Defines ================================================================== */
#define LINE_ENDING   "\r\n"
#define LINE_BUF_SIZE 192

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/** Log file */
static vfs_file_t * log_file;

/* Private functions ======================================================== */
static void log_write(const uint8_t * buffer, size_t size) {
#if USE_LOG_RESET_CURSOR
  vfs_write(log_file, (const uint8_t *) "\r", 1);
#endif

  vfs_write(log_file, buffer, size);
}

static const char * log_get_level_color(log_level_t level) {
#if USE_COLOR_LOG
  switch (level) {
    case LOG_DEBUG:   return COLOR_CYAN;
    case LOG_INFO:    return COLOR_BLUE;
    case LOG_WARNING: return COLOR_YELLOW;
    case LOG_ERROR:   return COLOR_RED;
    case LOG_FATAL:   return COLOR_RED_BG;
    default:
      return "";
  }
#else
  return "";
#endif
}

static const char * log_get_level_string(log_level_t level) {
  switch (level) {
    case LOG_DEBUG:   return "debug";
    case LOG_INFO:    return "info ";
    case LOG_WARNING: return "warn ";
    case LOG_ERROR:   return "error";
    case LOG_FATAL:   return "fatal";
    default:
      return "";
  }
}

/* Shared functions ========================================================= */
error_t log_init(vfs_file_t * out) {
  log_file = out;
  return E_OK;
}

log_level_t log_level_from_str(const char * str) {
  if (!strcmp(str, "debug")) {
    return LOG_DEBUG;
  } else if (!strcmp(str, "info")) {
    return LOG_INFO;
  } else if (!strcmp(str, "warn")) {
    return LOG_WARNING;
  } else if (!strcmp(str, "error")) {
    return LOG_ERROR;
  } else if (!strcmp(str, "fatal")) {
    return LOG_FATAL;
  }
  return LOG_DEBUG;
}

void vlog_fmt(log_level_t level, const char * fmt, va_list args) {
  char buf[LINE_BUF_SIZE];
  size_t size = 0;

  size += snprintf(buf + size, sizeof(buf) - size, "[%s%s%s] ",
                   log_get_level_color(level),
                   log_get_level_string(level),
                   USE_COLOR_LOG ? COLOR_RESET : "");
  size += vsnprintf(buf + size, sizeof(buf) - size, fmt, args);
  size += snprintf(buf + size, sizeof(buf) - size, LINE_ENDING);

  log_write((const uint8_t *) buf, size);
}

void vlog_module_fmt(log_level_t level, const char * tag, const char * fmt, va_list args) {
  char buf[LINE_BUF_SIZE];
  size_t size = 0;

  size += snprintf(buf + size, sizeof(buf) - size, "[%s%s%s] [%s%s%s] ",
                   log_get_level_color(level),
                   log_get_level_string(level),
                   USE_COLOR_LOG ? COLOR_RESET : "",
                   USE_COLOR_LOG ? COLOR_MAGENTA : "",
                   tag,
                   USE_COLOR_LOG ? COLOR_RESET : "");
  size += vsnprintf(buf + size, sizeof(buf) - size, fmt, args);
  size += snprintf(buf + size, sizeof(buf) - size, LINE_ENDING);

  log_write((const uint8_t *) buf, size);
}

void log_fmt(log_level_t level, const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vlog_fmt(level, fmt, args);
  va_end(args);
}

void log_module_fmt(log_level_t level, const char * tag, const char * fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vlog_module_fmt(level, tag, fmt, args);
  va_end(args);
}

void log_printf(const char * fmt, ...) {
  char buf[LINE_BUF_SIZE];

  va_list args;
  va_start(args, fmt);
  size_t size = vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  vfs_write(log_file, buf, size);
}
