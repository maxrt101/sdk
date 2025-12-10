#include <reent.h>
#include <stdio.h>

#include "vfs/vfs.h"
#include "shell/shell.h"
#include "posix/posix.h"

// struct _reent impure_ptr = {0};

// struct _reent * _impure_ptr = &impure_ptr;

custom_reent_t * __custom_reent;

custom_reent_t custom_reent;

void posix_reent_init(vfs_file_t * stdio, shell_t * shell) {
  _impure_ptr->_stdout = (struct __sFILE*) stdio;
  _impure_ptr->_stdin  = (struct __sFILE*) stdio;
  _impure_ptr->_stderr = (struct __sFILE*) stdio;

  __custom_reent = &custom_reent;
  __custom_reent->shell = shell;
}