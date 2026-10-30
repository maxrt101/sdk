#pragma once

#include <lds.h>
#include <util.h>
#include <stdint.h>

/**
 * @brief Name of a static variable which the ptr to file's logger is assigned
 *        to for log_* macros to work without specifying the logger
 */
#define LOGGER_GLOBAL_PTR_NAME logger_ptr__

/**
 * @brief Declare a local logger for the module
 *
 * @note __name must be a token (not a string)
 * @note Optional second argument is for specifying default minimal log level
 *
 * @warning __name must be unique across the project, if you need to use
 *          similarly named logger in different files - use LOGGER_DECLARE
 *          once, and LOGGER_USE in all other files
 */
#define LOGGER_DECLARE(__name, ...) \
  static logger_runtime_t UTIL_CAT(__name, _logger_runtime) = {               \
   .level = UTIL_DEFAULT_OR_VA_ARGS(LOG_INFO, __VA_ARGS__)                    \
  };                                                                          \
  LDS_APPEND_NAMED(loggers, UTIL_CAT(__name, _logger), {                      \
    .name = UTIL_STRINGIFY(__name),                                           \
    .rt = &UTIL_CAT(__name, _logger_runtime)                                  \
  });                                                                         \
  static const logger_t * LOGGER_GLOBAL_PTR_NAME = &UTIL_CAT(__name, _logger)

/**
 * @brief Used to "import" a logger, defined somewhere else
 */
#define LOGGER_USE(__name) \
  extern const logger_t UTIL_CAT(_name, _logger);                             \
  static const logger_t * LOGGER_GLOBAL_PTR_NAME = &UTIL_CAT(_name, _logger)

/**
 * @brief Helper to call the backend printf function with __VA_ARGS__ counted
 */
#define log_call_backend(__logger, __level, __fmt, ...) \
  log_backend_printf(__logger, __level, UTIL_VA_ARGS_COUNT(__VA_ARGS__), __fmt, ##__VA_ARGS__)

/**
 * @brief Calls logger backend with global (per-file) logger pointer
 */
#define log_call_backend_with_global_logger(__level, __fmt, ...) \
  log_call_backend(LOGGER_GLOBAL_PTR_NAME, __level, __fmt, ##__VA_ARGS__)

/**
 * @brief Call logger backend without a logger instance
 */
#define log_printf(__level, __fmt, ...) \
  log_call_backend(NULL, __level, __fmt, ##__VA_ARGS__)

#define log_debug(__fmt, ...)   log_call_backend_with_global_logger(LOG_DEBUG,   __fmt, ##__VA_ARGS__)
#define log_info(__fmt, ...)    log_call_backend_with_global_logger(LOG_INFO,    __fmt, ##__VA_ARGS__)
#define log_warning(__fmt, ...) log_call_backend_with_global_logger(LOG_WARNING, __fmt, ##__VA_ARGS__)
#define log_error(__fmt, ...)   log_call_backend_with_global_logger(LOG_ERROR,   __fmt, ##__VA_ARGS__)
#define log_fatal(__fmt, ...)   log_call_backend_with_global_logger(LOG_FATAL,   __fmt, ##__VA_ARGS__)

/**
 * @brief Log level
 */
typedef enum log_level {
  /// Used only by min log level restrictions
  LOG_DISABLED = -1,

  /// Actual log levels
  LOG_DEBUG = 0,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
  LOG_FATAL,

  /// Special marker, reserved
  LOG_MAX,
} log_level_t;

/**
 * @brief Runtime part of logger - configuration
 */
typedef struct logger_runtime {
  int8_t level;
} logger_runtime_t;

/**
 * @brief Static part of logger - descriptor
 */
typedef struct logger {
  const char *       name;
  logger_runtime_t * rt;
} logger_t;

/**
 * @brief Logger descriptors live in an LDS
 */
LDS_DECLARE(loggers, logger_t);

/**
 * @brief Return a ANSI color string given a log level
 */
const char * log_level2color(log_level_t level);

/**
 * @brief Return a string representation of a log level
 */
const char * log_level2str(log_level_t level);

/**
 * @brief Port function which actually forms & prints the log line
 *
 * @note Defined as __WEAK in log.c - made to be redefined
 */
void log_backend_printf(const logger_t * logger, log_level_t level, uint8_t argc, const char * fmt, ...)
  __PRINTF_LIKE(4, 5);
