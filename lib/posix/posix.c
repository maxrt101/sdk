#include "vfs/vfs.h"
#include "shell/shell.h"
#include "posix/posix.h"

void posix_init(vfs_file_t * stdio, shell_t * shell) {
  posix_reent_init(stdio, shell);
}
