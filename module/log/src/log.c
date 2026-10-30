#include <log.h>
#include <util.h>
#include <tty/ansi.h>

LDS_DEFINE(loggers);

const char * log_level2color(log_level_t level) {
  switch (level) {
    case LOG_DEBUG:   return LOG_DEBUG_COLOR;
    case LOG_INFO:    return LOG_INFO_COLOR;
    case LOG_WARNING: return LOG_WARNING_COLOR;
    case LOG_ERROR:   return LOG_ERROR_COLOR;
    case LOG_FATAL:   return LOG_FATAL_COLOR;
    default:          return "";
  }
}

const char * log_level2str(log_level_t level) {
  switch (level) {
    case LOG_DEBUG:   return "debug";
    case LOG_INFO:    return "info ";
    case LOG_WARNING: return "warn ";
    case LOG_ERROR:   return "error";
    case LOG_FATAL:   return "fatal";
    default:          return "?";
  }
}

__WEAK void log_backend_printf(const logger_t * logger, log_level_t level, uint8_t argc, const char * fmt, ...) {
  // Reimplement to enable logging
}
