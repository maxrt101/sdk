#include <util.h>
#include <error.h>

const char * err2str(error_t err) {
  switch (err) {
#define X(__name, ...) case __name: return #__name;
#include <util/errors.def>
#undef X
    default: return "?";
  }
}
