#pragma once

#include <device.h>
#include <stdio.h>
#include <termios.h>

typedef struct dev_cio_ctx {
  FILE *         out;
  FILE *         in;
  bool           is_non_blocking;
  struct termios orig_tty;
} dev_cio_ctx_t;

DEVICE_API_EXPORT(cio);
