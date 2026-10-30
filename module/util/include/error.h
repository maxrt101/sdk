#pragma once

#include <errno.h>
#include <util.h>

typedef enum error {
#define X(__name, ...) __name UTIL_IF_EMPTY(__VA_ARGS__, , = __VA_ARGS__),
#include <util/errors.def>
#undef X
} error_t;

const char * err2str(error_t err);
