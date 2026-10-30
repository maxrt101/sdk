#pragma once

/**
 * @brief Supress 'unused variable' and 'unused parameter' warnings
 */
#define UTIL_UNUSED(__x) (void)(__x)

/**
 * @brief Force an expansion
 */
#define UTIL_EXPAND(...) __VA_ARGS__

/**
 * @brief Force indirect expansion
 */
#define UTIL_EXPAND_TRAMPOLINE(...) UTIL_EXPAND __VA_ARGS__

/**
 * @brief Concatenates 2 tokens
 */
#define UTIL_CAT_RAW(__a, __b) __a ## __b

/**
 * @brief Concatenates 2 tokens, deferred
 */
#define UTIL_CAT(__a, __b) UTIL_CAT_RAW(__a, __b)

/**
 * @brief Concatenates an arbitrary number of tokens (limited by UTIL_REDUCE range)
 *
 * @note Dependant on UTIL_REDUCE from util/iter.h
 */
#define UTIL_CAT_ALL(...) UTIL_REDUCE(UTIL_CAT, __VA_ARGS__)

/**
 * @brief Determines smallest number from the two
 */
#define UTIL_MIN(__a, __b) \
  ({                                                                          \
    typeof(__a) _a = (__a);                                                   \
    typeof(__b) _b = (__b);                                                   \
    _a < _b ? _a : _b;                                                        \
  })

/**
 * @brief Determines biggest number from the 2
 */
#define UTIL_MAX(__a, __b) \
  ({                                                                          \
    typeof(__a) _a = (__a);                                                   \
    typeof(__b) _b = (__b);                                                   \
    _a > _b ? _a : _b;                                                        \
  })

/**
 * @brief Calculates size of an array
 */
#define UTIL_ARR_SIZE(__arr) (sizeof(__arr) / sizeof(__arr[0]))

/**
 * @brief Caps x between min and max
 */
#define UTIL_CAP(__x, __min, __max) \
    ((__x) < (__min) ? (__min) : ((__x) > (__max) ? (__max) : (__x)))
