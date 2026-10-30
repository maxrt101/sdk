#pragma once

#include <util.h>

#if has_module(log)
#include <log.h>
#else
#define LOGGER_DECLARE(...)
#define LOGGER_USE(...)

#define log_printf(...)
#define log_debug(...)
#define log_info(...)
#define log_warning(...)
#define log_error(...)
#define log_fatal(...)
#endif

