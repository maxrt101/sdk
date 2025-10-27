/** ========================================================================= *
 *
 * @file test.h
 * @date 10-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Simple testing framework/library
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "tty/ansi.h"
#include <stdbool.h>
#include <stddef.h>

/**
 * Include header needed by TEST_LOG_PORT, if exists
 */
#ifdef TEST_LOG_PORT_INC
#include TEST_LOG_PORT_INC
#endif

/* Defines ================================================================== */
/**
 * Log print function
 */
#ifndef TEST_LOG_PORT
#define TEST_LOG_PORT log_printf
#endif

/**
 * Shorthand for formatting alloc/free location (function:line)
 */
#define TEST_LOC_FMT                                                            \
  ANSI_COLOR_FG_CYAN "%s" ANSI_TEXT_RESET ":" ANSI_COLOR_FG_MAGENTA "%d" ANSI_TEXT_RESET

/* Macros =================================================================== */
/**
 * Base ASSERT macro for tests
 *
 * @param __value Expression that is expected to evaluate to true
 * @param __fail_str Fail message
 */
#define TEST_ASSERT(__value, __fail_str)                                        \
  do {                                                                          \
    if (!(__value)) {                                                           \
      TEST_LOG_PORT(ANSI_COLOR_FG_RED "ASSERT" ANSI_TEXT_RESET " '"             \
            ANSI_TEXT_BOLD "%s" ANSI_TEXT_RESET                                 \
            "' failed at " TEST_LOC_FMT " with message '"                       \
            ANSI_TEXT_BOLD "%s" ANSI_TEXT_RESET "'\n",                          \
        UTIL_STRINGIFY(__value), __FILE__, __LINE__, __fail_str);               \
      return false;                                                             \
    }                                                                           \
  } while (0)

/**
 * Assert than __a and __b are equal
 *
 * Calls TEST_ASSERT
 *
 * @param __a Value a
 * @param __b Value b
 * @param __fail_str Fail message
 */
#define TEST_ASSERT_EQ(__a, __b, __fail_str)                                    \
  TEST_ASSERT((__a) == (__b), __fail_str)

/**
 * Assert than __a and __b are not equal
 *
 * Calls TEST_ASSERT
 *
 * @param __a Value a
 * @param __b Value b
 * @param __fail_str Fail message
 */
#define TEST_ASSERT_NEQ(__a, __b, __fail_str)                                   \
  TEST_ASSERT((__a) != (__b), __fail_str)

/**
 * Assert than strings __a and __b are equal
 *
 * Calls TEST_ASSERT
 *
 * @param __a Value a
 * @param __b Value b
 * @param __fail_str Fail message
 */
#define TEST_ASSERT_STR_EQ(__a, __b, __fail_str)                                \
  TEST_ASSERT(!strcmp(__a, __b), __fail_str)

/**
 * Assert than __e is equal to E_OK
 *
 * Calls TEST_ASSERT
 *
 * @param __e Expression that evaluates to error_t
 * @param __fail_str Fail message
 */
#define TEST_ASSERT_ERROR(__e, __fail_str)                                      \
  TEST_ASSERT((__e) == E_OK, __fail_str)

/**
 * Declare test suite
 *
 * @param __name Test suite name
 * @param __size Max tests for this suite
 */
#define TEST_SUITE_DECLARE(__name, __size)                                      \
  test_t test_buffer_ ## __name[__size] = {0};                                  \
  tests_suite_t __name = {                                                      \
    test_buffer_ ## __name,                                                     \
    __size,                                                                     \
  };

/**
 * Declare test in a suite
 *
 * Example:
 * @code{.c}
 * TEST_SUITE_DECLARE(test_suite);
 * TEST_DECLARE(test_suite, test_name) {
 *   TEST_ASSERT(10 == 20, "values are not the same");
 * }
 * @endcode
 *
 * @param __suite Test suite name
 * @param __name Test name (identifier - no spaces, not special symbols)
 */
#define TEST_DECLARE(__suite, __name)                                           \
  bool test_ ## __name (tests_suite_t * suite);                                 \
  __CONSTRUCTOR(255) void __init_ ## __name(void) {                             \
    __suite.tests[__suite.size].fn = test_ ## __name;                           \
    __suite.tests[__suite.size].name = #__name;                                 \
    __suite.size++;                                                             \
  }                                                                             \
  bool test_ ## __name (tests_suite_t * suite)

/**
 * Use in tests to log, won't do anything if quiet flag is true
 *
 * @warning Can only be called from a test
 */
#define TEST_LOG(...)                                                           \
  do {                                                                          \
    if (!suite->context.quiet) {                                                \
      TEST_LOG_PORT(__VA_ARGS__);                                               \
    }                                                                           \
  } while (0)

/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Forward declaration of test suite
 */
typedef struct tests_suite_s tests_suite_t;

/**
 * Actual test function type
 */
typedef bool (*test_fn_t)(tests_suite_t *);

/**
 * Test context
 */
typedef struct {
  test_fn_t fn;
  const char * name;
} test_t;

/**
 * Tests (test suite) context
 */
typedef struct tests_suite_s {
  test_t * tests;
  size_t capacity;
  size_t size;

  struct {
    bool quiet;
  } context;
} tests_suite_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Runs all tests in global suit `tests`
 *
 * Example:
 * @code{.c}
 * TEST_SUITE_DECLARE(test_suite);
 * TEST_DECLARE(test_suite, test_name) {
 *   TEST_ASSERT(10 == 20, "values are not the same");
 * }
 * int main(int argc, char ** argv) {
 *   return tests_run(argc, argv);
 * }
 * @endcode
 *
 * @param argc Command line argument count
 * @param argv Command line argument array
 * @return Number of failed tests
 */
int tests_run(tests_suite_t * suite, int argc, char ** argv);

#ifdef __cplusplus
}
#endif