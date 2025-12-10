#include <stdlib.h>
#include "os/alloc/alloc.h"

void * malloc(size_t size) {
  return os_alloc(size);
}

void free(void * ptr) {
  os_free(ptr);
}
