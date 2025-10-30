/** ========================================================================= *
*
 * @file builtin_mem.c
 * @date 30-10-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief 'mem' builtin cli command implementation
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "shell/shell.h"
#include "shell/shell_util.h"
#include "time/sleep.h"
#include "error/assertion.h"
#include "log/log.h"
#include "os/alloc/alloc.h"

/* Defines ================================================================== */
#define LOG_TAG shell

/* Macros =================================================================== */
#define SH_MEM_HEXDUMP_LINE 16

/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/** Start of os heap, defined in linker */
extern uint32_t __os_heap_start;

/** End of os heap, defined in linker */
extern uint32_t __os_heap_end;

/* Private functions ======================================================== */
/* Shared functions ========================================================= */
int8_t builtin_mem(shell_t * sh, uint8_t argc, const char ** argv) {
  if (argc > 1) {
    if (!strcmp(argv[1], "read") || !strcmp(argv[1], "r")) {
      if (argc != 4) {
        log_error("Usage: mem read ADDR COUNT");
        return SHELL_FAIL;
      }

      uint32_t addr = shell_parse_int(argv[2]);
      uint32_t count = shell_parse_int(argv[3]);

      uint8_t * data = (uint8_t*) addr;

      log_printf("0x%08x: ", addr);
      for (size_t i = 0; i < count; ++i) {
        log_printf("%02x ", data[i]);

        if ((i+1) % SH_MEM_HEXDUMP_LINE == 0) {
          log_printf("\r\n");
          if (i + 1 != count) {
            log_printf("0x%08x: ", addr + i);
          }
        }
      }

      log_printf("\r\n");

      return SHELL_OK;
    }

    log_error("Usage: mem [read] ...");
    return SHELL_FAIL;
  }

  os_heap_t * heap = os_get_heap();

  log_info("OS Heap: %d/%d bytes", heap->used, heap->size);

  return SHELL_OK;

}
