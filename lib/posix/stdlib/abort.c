#include <stdlib.h>
#include "os/abort/abort.h"

void abort(void) {
  os_abort("Abort");
}
