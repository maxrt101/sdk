/** ========================================================================= *
 *
 * @file assertion.h
 * @date 20-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/util.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Calls error_handler with source file and line, where the error occurred
 */
#define ERROR_HANDLER_CALL(err) error_handler(err, __LINE__, __FILE__)

/**
 * Asserts expression, if failed calls error_handler
 */
#define ASSERT(expr)                  \
    do {                              \
      if (!(expr)) {                  \
        ERROR_HANDLER_CALL(E_ASSERT); \
      }                               \
    } while (0)

/**
 * Asserts expression, if failed executes user code
 */
#define ASSERT_OR_ELSE(expr, ...) \
    do {                          \
      if (!(expr)) {              \
        __VA_ARGS__;              \
      }                           \
    } while (0)

/**
 * Asserts expression, if failed returns
 */
#define ASSERT_RETURN(expr, ...) \
    do {                         \
      if (!(expr)) {             \
        return __VA_ARGS__;      \
      }                          \
    } while (0)

/**
 * Implementation of ERROR_CHECK/ERROR_CHECK_RETURN
 */
#define ERROR_CHECK_IMPL_CHECK(error_var, ...)    \
    do {                                          \
      if (error_var != E_OK) {                    \
        __VA_ARGS__;                              \
      }                                           \
    } while (0)

/**
 * Implementation of ERROR_CHECK/ERROR_CHECK_RETURN
 */
#define ERROR_CHECK_IMPL(error_var, expr, ...)                                        \
    do {                                                                              \
       error_t error_var = expr;                                                      \
       ERROR_CHECK_IMPL_CHECK(error_var,                                              \
          UTIL_IF_EMPTY(__VA_ARGS__,                                                  \
                        ERROR_HANDLER_CALL(error_var),                                \
                        __VA_ARGS__; ERROR_HANDLER_CALL(error_var)));                 \
    } while (0)

/**
 * Checks expression for errors, if not E_OK, calls error_handler
 *
 * @param expr Expression to check
 * @param ... Code to execute if check failed
 */
#define ERROR_CHECK(expr, ...) \
    ERROR_CHECK_IMPL(UTIL_CAT(__err, __LINE__), expr, __VA_ARGS__)

/**
 * Checks expression for errors, if not E_OK, returns error
 *
 * @param expr Expression to check
 * @param ... Code to execute if check failed
 */
#define ERROR_CHECK_RETURN(expr, ...)                                     \
    do {                                                                  \
       error_t error_var = expr;                                          \
       ERROR_CHECK_IMPL_CHECK(error_var,                                  \
          UTIL_IF_EMPTY(__VA_ARGS__,                                      \
                        return error_var,                                 \
                        __VA_ARGS__; return error_var));                  \
    } while (0)

/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */

#ifdef __cplusplus
}
#endif

