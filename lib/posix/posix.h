#pragma once

#include "vfs/vfs.h"
#include "shell/shell.h"


typedef struct {
  shell_t * shell;
} custom_reent_t;

extern custom_reent_t * __custom_reent;

void posix_init(vfs_file_t * stdio, shell_t * shell);

void posix_reent_init(vfs_file_t * stdio, shell_t * shell);
