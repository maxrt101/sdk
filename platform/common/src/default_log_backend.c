#if WITH_DEFAULT_LOG_BACKEND

#include <log.h>
#include <tty.h>
#include <tty/ansi.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

// Any of the following must define DEFAULT_LOG_BACKEND_TTY & provide the symbol
#include <platform.h>
#include <target.h>

#ifndef DEFAULT_LOG_BACKEND_TTY
#error "DEFAULT_LOG_BACKEND_TTY is required for WITH_DEFAULT_LOG_BACKEND=1"
#endif

void log_backend_printf(const logger_t * logger, log_level_t level, __UNUSED uint8_t argc, const char * fmt, ...) {
  if (logger && level < logger->rt->level) {
    return;
  }

  if (!logger && level < LOG_LEVEL_GLOBAL_MIN) {
    return;
  }

  va_list ap;
  va_start(ap, fmt);

  char buf[TARGET_MAX_LOG_LINE] = {'\r'};
  size_t size = 1;

  if (logger) {
    size += snprintf(
      buf + size, sizeof(buf) - size - 1, "[%s%s%s][%s%s%s] ",
      log_level2color(level),
      log_level2str(level),
      WITH_TTY_ANSI_COLORS ? ANSI_TEXT_RESET : "",
      WITH_TTY_ANSI_COLORS ? ANSI_COLOR_FG_MAGENTA : "",
      logger->name,
      WITH_TTY_ANSI_COLORS ? ANSI_TEXT_RESET : ""
    );
  } else {
    size += snprintf(
      buf + size, sizeof(buf) - size - 1, "[%s%s%s] ",
      log_level2color(level),
      log_level2str(level),
      WITH_TTY_ANSI_COLORS ? ANSI_TEXT_RESET : ""
    );
  }

  size += vsnprintf(buf + size, sizeof(buf) - size - 1, fmt, ap);
  size += snprintf(buf + size, sizeof(buf) - size - 1, "\r\n");

  buf[size] = 0;

  tty_write(DEFAULT_LOG_BACKEND_TTY, buf, size);

  va_end(ap);
}

#endif
