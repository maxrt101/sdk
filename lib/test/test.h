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
#include "log/color.h"
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
 * Max tests number. Needed because TEST_DECLARE uses constructors and static
 * array to store tests
 */
#ifndef TESTS_MAX
#define TESTS_MAX     32
#endif

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
  COLOR_CYAN "%s" COLOR_RESET ":" COLOR_MAGENTA "%d" COLOR_RESET

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
      TEST_LOG_PORT(COLOR_RED "ASSERT" COLOR_RESET " '"                         \
            COLOR_BOLD "%s" COLOR_RESET                                         \
            "' failed at " TEST_LOC_FMT " with message '"                       \
            COLOR_BOLD "%s" COLOR_RESET "'\n",                                  \
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
 * Declare test
 *
 * Example:
 * @code{.c}
 * TEST_DECLARE(test_name) {
 *   TEST_ASSERT(10 == 20, "values are not the same");
 * }
 * @endcode
 *
 * @param __name Test name (identifier - no spaces, not special symbols)
 */
#define TEST_DECLARE(__name)                                                    \
  bool test_ ## __name ();                                                      \
  __CONSTRUCTOR(255) void __init_ ## __name(void) {                             \
    tests.tests[tests.size].fn = test_ ## __name;                               \
    tests.tests[tests.size].name = #__name;                                     \
    tests.size++;                                                               \
  }                                                                             \
  bool test_ ## __name ()

/**
 * Use in tests to log, won't do anything if quiet flag is true
 */
#define TEST_LOG(...)                                                           \
  do {                                                                          \
    if (!tests.context.quiet) {                                                 \
      TEST_LOG_PORT(__VA_ARGS__);                                               \
    }                                                                           \
  } while (0)

/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Actual test function type
 */
typedef bool (*test_fn_t)(void);

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
typedef struct {
  size_t size;
  test_t tests[TESTS_MAX];

  struct {
    bool quiet;
  } context;
} tests_ctx_t;

/* Variables ================================================================ */
/**
 * Every test suite (collection of tests in tests_ctx_t) need to define
 * global variable `tests` so that TEST_DECLARE knows where to put new tests
 *
 * TODO: Get rid of this, create TEST_SUIT_DECLARE(suit) &
 *       TEST_DECLARE(suit, test) & tests_run(&suit, argc, argv)
 */
extern tests_ctx_t tests;

/* Shared functions ========================================================= */
/**
 * Runs all tests in global suit `tests`
 *
 * @param argc Command line argument count
 * @param argv Command line argument array
 * @return Number of failed tests
 */
int tests_run(int argc, char ** argv);

#ifdef __cplusplus
}
#endif