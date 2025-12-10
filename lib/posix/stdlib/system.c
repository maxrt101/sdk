#include <stdlib.h>
#include "shell/shell.h"
#include "posix/posix.h"

int system(const char * cmd) {
  shell_execute(__custom_reent->shell, cmd);
}
