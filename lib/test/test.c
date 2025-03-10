/** ========================================================================= *
 *
 * @file test.c
 * @date 10-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "test/test.h"
#include "log/log.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
int tests_run(tests_suite_t * suite, int argc, char ** argv) {
  suite->context.quiet = false;

  for (size_t i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-h")) {
      TEST_LOG_PORT(
        "Usage: %s [-h] [-q]\r\n"
        "  -h - Shows this help\r\n"
        "  -q - Don't print debug logs\r\n",
        argv[0]
      );
      return 0;
    } else if (!strcmp(argv[i], "-q")) {
      suite->context.quiet = true;
    } else {
      TEST_LOG_PORT("Unrecognized argument '%s'\r\n", argv[i]);
    }
  }

  TEST_LOG_PORT("Current test run: %zu\n\n", suite->size);

  size_t passed = 0;
  size_t failed = 0;

  for (size_t i = 0; i < suite->size; ++i) {
    TEST_LOG(COLOR_BOLD "========== TEST %s ==========\n" COLOR_RESET,
      suite->tests[i].name);

    bool result = suite->tests[i].fn(suite);

    if (result) {
      TEST_LOG_PORT("[" COLOR_GREEN "PASS" COLOR_RESET "] %s\n",
        suite->tests[i].name);
      passed++;
    } else {
      TEST_LOG_PORT("[" COLOR_RED "FAIL" COLOR_RESET "] %s\n",
        suite->tests[i].name);
      failed++;
    }

    TEST_LOG("\n");
  }

  TEST_LOG_PORT("Summary: "
    COLOR_BOLD  "%zu" COLOR_RESET " tests " COLOR_GREEN "passed" COLOR_RESET ", "
    COLOR_BOLD "%zu" COLOR_RESET " tests " COLOR_RED "failed" COLOR_RESET
    "\n", passed, failed);

  return (int) failed;
}

