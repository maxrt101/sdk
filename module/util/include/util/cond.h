#pragma once

#include <util/base.h>
#include <util/vargs.h>


/**
 * @brief Helper macro for UTIL_IF_1
 */
#define UTIL_IF1_1 __X__,

/**
 * @brief Expanded to __if_true if __cond is 1, __VA_ARGS__ otherwise
 *
 * Works by concatenating value '__cond' with UTIL_IF1_, if '__cond'
 * is 1 - it will produce UTIL_IF1_1, which will produce '__X__,' and
 * second argument will be __if_true, if '__cond' is anything but 1
 * (let's say 0), UTIL_IF1_0 is empty, and doesn't contain a comma, so
 * second argument will be __VA_ARGS__, and it will get called
 */
#define UTIL_IF_1_IMPL(__cond, __if_true, ...) \
    UTIL_EXPAND(UTIL_GET_ARG2_OBSTRUCT(UTIL_CAT(UTIL_IF1_, __cond) __if_true, __VA_ARGS__))

/**
 * @brief Expanded to __if_true if __cond is 1, __VA_ARGS__ otherwise
 *
 * @note __cond must be literally 1
 */
#define UTIL_IF_1(__cond, __if_true, ...) \
    UTIL_IF_1_IMPL(__cond, __if_true, __VA_ARGS__)

/**
 * @brief Same as UTIL_IF_1, but arguments must be enclosed in ()
 *
 * @note Useful when a comma has to be a part of an argument
 */
#define UTIL_IF_1P(__cond, __if_true, __if_false) \
  UTIL_EXPAND_TRAMPOLINE(UTIL_IF_1(__cond, __if_true, __if_false))


/**
 * @brief Helper macro for UTIL_IF_0
 */
#define UTIL_IF0_0 __X__,

/**
 * @brief Expanded to __if_true if __cond is 0, __VA_ARGS__ otherwise
 *
 * Works by concatenating value '__cond' with UTIL_IF0_, if '__cond'
 * is 0 - it will produce UTIL_IF0_0, which will produce '__X__,' and
 * second argument will be __if_true, if '__cond' is anything but 0
 * (let's say 1), UTIL_IF0_1 is empty, and doesn't contain a comma, so
 * second argument will be __VA_ARGS__, and it will get called
 */
#define UTIL_IF_0_IMPL(__cond, __if_true, ...) \
    UTIL_EXPAND(UTIL_GET_ARG2_OBSTRUCT(UTIL_CAT(UTIL_IF0_, __cond) __if_true, __VA_ARGS__))

/**
 * @brief Expanded to __if_true if __cond is 0, __VA_ARGS__ otherwise
 *
 * @note __cond must be literally 0
 */
#define UTIL_IF_0(__cond, __if_true, ...) \
    UTIL_IF_0_IMPL(__cond, __if_true, __VA_ARGS__)

/**
 * @brief Same as UTIL_IF_0, but arguments must be enclosed in ()
 *
 * @note Useful when a comma has to be a part of an argument
 */
#define UTIL_IF_0P(__cond, __if_true, __if_false) \
  UTIL_EXPAND_TRAMPOLINE(UTIL_IF_0(__cond, __if_true, __if_false))


/**
 * @brief Helper macro for UTIL_IF_NON0
 */
#define UTIL_IF_NON0_0 __X__,

/**
 * @brief Expanded to __if_true if __cond is anything but 0, __if_false otherwise
 *
 * Works by concatenating value '__cond' with UTIL_COND_NON0_, if
 * '__cond' is 0 - it will produce UTIL_COND_NON0_0, which will
 * produce '__X__,' and second argument will be __if_false, if '__cond'
 * is anything but 0 (let's say 1), UTIL_COND_NON0_1 is empty, and
 * doesn't contain a comma, so second argument will be __if_true, and
 * it will get called
 */
#define UTIL_IF_NON0(__cond, __if_true, __if_false) \
    UTIL_EXPAND(UTIL_GET_ARG2_OBSTRUCT(UTIL_CAT(UTIL_IF_NON0_, __cond) __if_false, __if_true))


/**
 * @brief Returns the 16th argument
 */
#define UTIL_ARG_16(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, N, ...) N

/**
 * @brief Evaluates to 1 if __VA_ARGS__ contains a top-level comma, 0 otherwise
 */
#define UTIL_HAS_COMMA(...) \
    UTIL_EXPAND(UTIL_ARG_16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, ~))

/**
 * @brief Triggers a comma when followed by ()
 */
#define UTIL_TRIGGER_PARENTHESIS(...) ,

/**
 * @brief Emptiness cases. Only 0001 evaluates to a comma
 */
#define UTIL_IS_EMPTY_CASE_0001 ,

/**
 * @brief Hardcoded 5-token paste
 *
 * Breaks the circular dependency with UTIL_VA_ARGS_COUNT
 */
#define UTIL_PASTE5(_1, _2, _3, _4, _5) _1 ## _2 ## _3 ## _4 ## _5

#define UTIL_IS_EMPTY_INDEX(_1, _2, _3, _4) UTIL_PASTE5(UTIL_IS_EMPTY_CASE_, _1, _2, _3, _4)

/**
 * @brief Returns 1 if conditions is empty, 0 otherwise.
 *
 * When empty, the tests evaluate to (0, 0, 0, 1)
 * UTIL_IS_EMPTY_INDEX(0, 0, 0, 1) evaluates to UTIL_IS_EMPTY_CASE_0001
 * UTIL_IS_EMPTY_CASE_0001 expands to just ,
 * The outer UTIL_HAS_COMMA( , ) immediately sees a comma and correctly evaluates to 1
 * For all other cases, it generates an undefined token (like UTIL_IS_EMPTY_CASE_0000),
 * which has no comma, so the outer UTIL_HAS_COMMA perfectly defaults to 0
 */
#define UTIL_IS_EMPTY(...)                                                     \
    UTIL_HAS_COMMA(                                                            \
        UTIL_EXPAND(UTIL_IS_EMPTY_INDEX(                                       \
            /* Is it already a comma? */                                       \
            UTIL_HAS_COMMA(__VA_ARGS__),                                       \
            /* Does it generate a comma when called with ()? */                \
            UTIL_HAS_COMMA(__VA_ARGS__ ()),                                    \
            /* Does it swallow ()? */                                          \
            UTIL_HAS_COMMA(UTIL_TRIGGER_PARENTHESIS __VA_ARGS__),              \
            /* Is it empty? */                                                 \
            UTIL_HAS_COMMA(UTIL_TRIGGER_PARENTHESIS __VA_ARGS__ ())            \
        ))                                                                     \
    )

/**
 * @brief Expanded to __if_true if __cond is empty, __if_false otherwise
 */
#define UTIL_IF_EMPTY(__cond, __if_true, __if_false) \
    UTIL_IF_1(UTIL_IS_EMPTY(__cond), __if_true, __if_false)
