#pragma once

#include <util/base.h>
#include <util/vargs.h>


/**
 * Code for generating the UTIL_FOR_EACH_* macros below:
 * @code{.py}
print("#define UTIL_FOR_EACH_IMPL_1(__macro, __x) __macro(__x)")
for i in range(2, 65):
  print(f"#define UTIL_FOR_EACH_IMPL_{i}(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_{i-1}(__macro, __VA_ARGS__)")
 * @endcode
 */

#define UTIL_FOR_EACH_IMPL_1(__macro, __x) __macro(__x)
#define UTIL_FOR_EACH_IMPL_2(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_1(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_3(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_2(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_4(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_3(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_5(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_4(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_6(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_5(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_7(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_6(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_8(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_7(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_9(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_8(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_10(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_9(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_11(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_10(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_12(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_11(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_13(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_12(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_14(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_13(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_15(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_14(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_16(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_15(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_17(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_16(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_18(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_17(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_19(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_18(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_20(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_19(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_21(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_20(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_22(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_21(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_23(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_22(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_24(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_23(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_25(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_24(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_26(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_25(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_27(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_26(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_28(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_27(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_29(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_28(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_30(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_29(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_31(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_30(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_32(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_31(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_33(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_32(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_34(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_33(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_35(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_34(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_36(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_35(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_37(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_36(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_38(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_37(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_39(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_38(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_40(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_39(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_41(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_40(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_42(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_41(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_43(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_42(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_44(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_43(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_45(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_44(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_46(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_45(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_47(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_46(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_48(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_47(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_49(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_48(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_50(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_49(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_51(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_50(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_52(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_51(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_53(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_52(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_54(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_53(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_55(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_54(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_56(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_55(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_57(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_56(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_58(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_57(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_59(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_58(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_60(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_59(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_61(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_60(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_62(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_61(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_63(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_62(__macro, __VA_ARGS__)
#define UTIL_FOR_EACH_IMPL_64(__macro, __x, ...) __macro(__x) UTIL_FOR_EACH_IMPL_63(__macro, __VA_ARGS__)

/**
 * @brief Macro for iterating over __VA_ARGS__ passing each of them into __macro
 *
 * Example:
 * @code{.c}
 *   #define PRINT(__x) printf("%s ", UTIL_STRINGIFY(__x));
 *   UTIL_FOR_EACH(PRINT, Apple, Banana, Orange);
 * @endcode
 *
 * @param __macro Macro that will receive each of the __VA_ARGS__ individually
 */
#define UTIL_FOR_EACH(__macro, ...) \
  UTIL_CAT(UTIL_FOR_EACH_IMPL_, UTIL_VA_ARGS_COUNT(__VA_ARGS__))(__macro, __VA_ARGS__)


/**
 * Code for generating the UTIL_ENUMERATE_* macros below:
 * @code{.py}
print("#define UTIL_ENUMERATE_IMPL_1(__total, __macro, __x) __macro((__total - 1), __x)")
for i in range(2, 65):
  print(f"#define UTIL_ENUMERATE_IMPL_{i}(__total, __macro, __x, ...) __macro((__total - {i}), __x) UTIL_ENUMERATE_IMPL_{i-1}(__total, __macro, __VA_ARGS__)")
 * @endcode
 */

#define UTIL_ENUMERATE_IMPL_1(__total, __macro, __x) __macro((__total - 1), __x)
#define UTIL_ENUMERATE_IMPL_2(__total, __macro, __x, ...) __macro((__total - 2), __x) UTIL_ENUMERATE_IMPL_1(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_3(__total, __macro, __x, ...) __macro((__total - 3), __x) UTIL_ENUMERATE_IMPL_2(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_4(__total, __macro, __x, ...) __macro((__total - 4), __x) UTIL_ENUMERATE_IMPL_3(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_5(__total, __macro, __x, ...) __macro((__total - 5), __x) UTIL_ENUMERATE_IMPL_4(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_6(__total, __macro, __x, ...) __macro((__total - 6), __x) UTIL_ENUMERATE_IMPL_5(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_7(__total, __macro, __x, ...) __macro((__total - 7), __x) UTIL_ENUMERATE_IMPL_6(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_8(__total, __macro, __x, ...) __macro((__total - 8), __x) UTIL_ENUMERATE_IMPL_7(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_9(__total, __macro, __x, ...) __macro((__total - 9), __x) UTIL_ENUMERATE_IMPL_8(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_10(__total, __macro, __x, ...) __macro((__total - 10), __x) UTIL_ENUMERATE_IMPL_9(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_11(__total, __macro, __x, ...) __macro((__total - 11), __x) UTIL_ENUMERATE_IMPL_10(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_12(__total, __macro, __x, ...) __macro((__total - 12), __x) UTIL_ENUMERATE_IMPL_11(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_13(__total, __macro, __x, ...) __macro((__total - 13), __x) UTIL_ENUMERATE_IMPL_12(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_14(__total, __macro, __x, ...) __macro((__total - 14), __x) UTIL_ENUMERATE_IMPL_13(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_15(__total, __macro, __x, ...) __macro((__total - 15), __x) UTIL_ENUMERATE_IMPL_14(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_16(__total, __macro, __x, ...) __macro((__total - 16), __x) UTIL_ENUMERATE_IMPL_15(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_17(__total, __macro, __x, ...) __macro((__total - 17), __x) UTIL_ENUMERATE_IMPL_16(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_18(__total, __macro, __x, ...) __macro((__total - 18), __x) UTIL_ENUMERATE_IMPL_17(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_19(__total, __macro, __x, ...) __macro((__total - 19), __x) UTIL_ENUMERATE_IMPL_18(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_20(__total, __macro, __x, ...) __macro((__total - 20), __x) UTIL_ENUMERATE_IMPL_19(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_21(__total, __macro, __x, ...) __macro((__total - 21), __x) UTIL_ENUMERATE_IMPL_20(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_22(__total, __macro, __x, ...) __macro((__total - 22), __x) UTIL_ENUMERATE_IMPL_21(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_23(__total, __macro, __x, ...) __macro((__total - 23), __x) UTIL_ENUMERATE_IMPL_22(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_24(__total, __macro, __x, ...) __macro((__total - 24), __x) UTIL_ENUMERATE_IMPL_23(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_25(__total, __macro, __x, ...) __macro((__total - 25), __x) UTIL_ENUMERATE_IMPL_24(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_26(__total, __macro, __x, ...) __macro((__total - 26), __x) UTIL_ENUMERATE_IMPL_25(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_27(__total, __macro, __x, ...) __macro((__total - 27), __x) UTIL_ENUMERATE_IMPL_26(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_28(__total, __macro, __x, ...) __macro((__total - 28), __x) UTIL_ENUMERATE_IMPL_27(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_29(__total, __macro, __x, ...) __macro((__total - 29), __x) UTIL_ENUMERATE_IMPL_28(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_30(__total, __macro, __x, ...) __macro((__total - 30), __x) UTIL_ENUMERATE_IMPL_29(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_31(__total, __macro, __x, ...) __macro((__total - 31), __x) UTIL_ENUMERATE_IMPL_30(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_32(__total, __macro, __x, ...) __macro((__total - 32), __x) UTIL_ENUMERATE_IMPL_31(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_33(__total, __macro, __x, ...) __macro((__total - 33), __x) UTIL_ENUMERATE_IMPL_32(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_34(__total, __macro, __x, ...) __macro((__total - 34), __x) UTIL_ENUMERATE_IMPL_33(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_35(__total, __macro, __x, ...) __macro((__total - 35), __x) UTIL_ENUMERATE_IMPL_34(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_36(__total, __macro, __x, ...) __macro((__total - 36), __x) UTIL_ENUMERATE_IMPL_35(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_37(__total, __macro, __x, ...) __macro((__total - 37), __x) UTIL_ENUMERATE_IMPL_36(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_38(__total, __macro, __x, ...) __macro((__total - 38), __x) UTIL_ENUMERATE_IMPL_37(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_39(__total, __macro, __x, ...) __macro((__total - 39), __x) UTIL_ENUMERATE_IMPL_38(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_40(__total, __macro, __x, ...) __macro((__total - 40), __x) UTIL_ENUMERATE_IMPL_39(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_41(__total, __macro, __x, ...) __macro((__total - 41), __x) UTIL_ENUMERATE_IMPL_40(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_42(__total, __macro, __x, ...) __macro((__total - 42), __x) UTIL_ENUMERATE_IMPL_41(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_43(__total, __macro, __x, ...) __macro((__total - 43), __x) UTIL_ENUMERATE_IMPL_42(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_44(__total, __macro, __x, ...) __macro((__total - 44), __x) UTIL_ENUMERATE_IMPL_43(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_45(__total, __macro, __x, ...) __macro((__total - 45), __x) UTIL_ENUMERATE_IMPL_44(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_46(__total, __macro, __x, ...) __macro((__total - 46), __x) UTIL_ENUMERATE_IMPL_45(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_47(__total, __macro, __x, ...) __macro((__total - 47), __x) UTIL_ENUMERATE_IMPL_46(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_48(__total, __macro, __x, ...) __macro((__total - 48), __x) UTIL_ENUMERATE_IMPL_47(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_49(__total, __macro, __x, ...) __macro((__total - 49), __x) UTIL_ENUMERATE_IMPL_48(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_50(__total, __macro, __x, ...) __macro((__total - 50), __x) UTIL_ENUMERATE_IMPL_49(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_51(__total, __macro, __x, ...) __macro((__total - 51), __x) UTIL_ENUMERATE_IMPL_50(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_52(__total, __macro, __x, ...) __macro((__total - 52), __x) UTIL_ENUMERATE_IMPL_51(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_53(__total, __macro, __x, ...) __macro((__total - 53), __x) UTIL_ENUMERATE_IMPL_52(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_54(__total, __macro, __x, ...) __macro((__total - 54), __x) UTIL_ENUMERATE_IMPL_53(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_55(__total, __macro, __x, ...) __macro((__total - 55), __x) UTIL_ENUMERATE_IMPL_54(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_56(__total, __macro, __x, ...) __macro((__total - 56), __x) UTIL_ENUMERATE_IMPL_55(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_57(__total, __macro, __x, ...) __macro((__total - 57), __x) UTIL_ENUMERATE_IMPL_56(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_58(__total, __macro, __x, ...) __macro((__total - 58), __x) UTIL_ENUMERATE_IMPL_57(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_59(__total, __macro, __x, ...) __macro((__total - 59), __x) UTIL_ENUMERATE_IMPL_58(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_60(__total, __macro, __x, ...) __macro((__total - 60), __x) UTIL_ENUMERATE_IMPL_59(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_61(__total, __macro, __x, ...) __macro((__total - 61), __x) UTIL_ENUMERATE_IMPL_60(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_62(__total, __macro, __x, ...) __macro((__total - 62), __x) UTIL_ENUMERATE_IMPL_61(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_63(__total, __macro, __x, ...) __macro((__total - 63), __x) UTIL_ENUMERATE_IMPL_62(__total, __macro, __VA_ARGS__)
#define UTIL_ENUMERATE_IMPL_64(__total, __macro, __x, ...) __macro((__total - 64), __x) UTIL_ENUMERATE_IMPL_63(__total, __macro, __VA_ARGS__)

/**
 * @brief Macro for iterating over __VA_ARGS__ passing each of them into __macro along with index
 *
 * Example:
 * @code{.c}
 *   #define PRINT(__i, __x) printf("[%d] %s\n", __i, UTIL_STRINGIFY(__x));
 *   UTIL_ENUMERATE(PRINT, Apple, Banana, Orange);
 * @endcode
 *
 * @param __macro Macro that will receive each of the __VA_ARGS__ individually along with index
 */
#define UTIL_ENUMERATE(__macro, ...) \
  UTIL_CAT(UTIL_ENUMERATE_IMPL_, UTIL_VA_ARGS_COUNT(__VA_ARGS__))(UTIL_VA_ARGS_COUNT(__VA_ARGS__), __macro, __VA_ARGS__)


/**
 * Code for generating the UTIL_REPEAT_* macros below:
 * @code{.py}
print("#define UTIL_REPEAT_IMPL_0(__macro)")
for i in range(1, 65):
  print(f"#define UTIL_REPEAT_IMPL_{i}(__macro) UTIL_REPEAT_IMPL_{i-1}(__macro) __macro({i - 1})")
 * @endcode
 */

#define UTIL_REPEAT_IMPL_0(__macro)
#define UTIL_REPEAT_IMPL_1(__macro) UTIL_REPEAT_IMPL_0(__macro) __macro(0)
#define UTIL_REPEAT_IMPL_2(__macro) UTIL_REPEAT_IMPL_1(__macro) __macro(1)
#define UTIL_REPEAT_IMPL_3(__macro) UTIL_REPEAT_IMPL_2(__macro) __macro(2)
#define UTIL_REPEAT_IMPL_4(__macro) UTIL_REPEAT_IMPL_3(__macro) __macro(3)
#define UTIL_REPEAT_IMPL_5(__macro) UTIL_REPEAT_IMPL_4(__macro) __macro(4)
#define UTIL_REPEAT_IMPL_6(__macro) UTIL_REPEAT_IMPL_5(__macro) __macro(5)
#define UTIL_REPEAT_IMPL_7(__macro) UTIL_REPEAT_IMPL_6(__macro) __macro(6)
#define UTIL_REPEAT_IMPL_8(__macro) UTIL_REPEAT_IMPL_7(__macro) __macro(7)
#define UTIL_REPEAT_IMPL_9(__macro) UTIL_REPEAT_IMPL_8(__macro) __macro(8)
#define UTIL_REPEAT_IMPL_10(__macro) UTIL_REPEAT_IMPL_9(__macro) __macro(9)
#define UTIL_REPEAT_IMPL_11(__macro) UTIL_REPEAT_IMPL_10(__macro) __macro(10)
#define UTIL_REPEAT_IMPL_12(__macro) UTIL_REPEAT_IMPL_11(__macro) __macro(11)
#define UTIL_REPEAT_IMPL_13(__macro) UTIL_REPEAT_IMPL_12(__macro) __macro(12)
#define UTIL_REPEAT_IMPL_14(__macro) UTIL_REPEAT_IMPL_13(__macro) __macro(13)
#define UTIL_REPEAT_IMPL_15(__macro) UTIL_REPEAT_IMPL_14(__macro) __macro(14)
#define UTIL_REPEAT_IMPL_16(__macro) UTIL_REPEAT_IMPL_15(__macro) __macro(15)
#define UTIL_REPEAT_IMPL_17(__macro) UTIL_REPEAT_IMPL_16(__macro) __macro(16)
#define UTIL_REPEAT_IMPL_18(__macro) UTIL_REPEAT_IMPL_17(__macro) __macro(17)
#define UTIL_REPEAT_IMPL_19(__macro) UTIL_REPEAT_IMPL_18(__macro) __macro(18)
#define UTIL_REPEAT_IMPL_20(__macro) UTIL_REPEAT_IMPL_19(__macro) __macro(19)
#define UTIL_REPEAT_IMPL_21(__macro) UTIL_REPEAT_IMPL_20(__macro) __macro(20)
#define UTIL_REPEAT_IMPL_22(__macro) UTIL_REPEAT_IMPL_21(__macro) __macro(21)
#define UTIL_REPEAT_IMPL_23(__macro) UTIL_REPEAT_IMPL_22(__macro) __macro(22)
#define UTIL_REPEAT_IMPL_24(__macro) UTIL_REPEAT_IMPL_23(__macro) __macro(23)
#define UTIL_REPEAT_IMPL_25(__macro) UTIL_REPEAT_IMPL_24(__macro) __macro(24)
#define UTIL_REPEAT_IMPL_26(__macro) UTIL_REPEAT_IMPL_25(__macro) __macro(25)
#define UTIL_REPEAT_IMPL_27(__macro) UTIL_REPEAT_IMPL_26(__macro) __macro(26)
#define UTIL_REPEAT_IMPL_28(__macro) UTIL_REPEAT_IMPL_27(__macro) __macro(27)
#define UTIL_REPEAT_IMPL_29(__macro) UTIL_REPEAT_IMPL_28(__macro) __macro(28)
#define UTIL_REPEAT_IMPL_30(__macro) UTIL_REPEAT_IMPL_29(__macro) __macro(29)
#define UTIL_REPEAT_IMPL_31(__macro) UTIL_REPEAT_IMPL_30(__macro) __macro(30)
#define UTIL_REPEAT_IMPL_32(__macro) UTIL_REPEAT_IMPL_31(__macro) __macro(31)
#define UTIL_REPEAT_IMPL_33(__macro) UTIL_REPEAT_IMPL_32(__macro) __macro(32)
#define UTIL_REPEAT_IMPL_34(__macro) UTIL_REPEAT_IMPL_33(__macro) __macro(33)
#define UTIL_REPEAT_IMPL_35(__macro) UTIL_REPEAT_IMPL_34(__macro) __macro(34)
#define UTIL_REPEAT_IMPL_36(__macro) UTIL_REPEAT_IMPL_35(__macro) __macro(35)
#define UTIL_REPEAT_IMPL_37(__macro) UTIL_REPEAT_IMPL_36(__macro) __macro(36)
#define UTIL_REPEAT_IMPL_38(__macro) UTIL_REPEAT_IMPL_37(__macro) __macro(37)
#define UTIL_REPEAT_IMPL_39(__macro) UTIL_REPEAT_IMPL_38(__macro) __macro(38)
#define UTIL_REPEAT_IMPL_40(__macro) UTIL_REPEAT_IMPL_39(__macro) __macro(39)
#define UTIL_REPEAT_IMPL_41(__macro) UTIL_REPEAT_IMPL_40(__macro) __macro(40)
#define UTIL_REPEAT_IMPL_42(__macro) UTIL_REPEAT_IMPL_41(__macro) __macro(41)
#define UTIL_REPEAT_IMPL_43(__macro) UTIL_REPEAT_IMPL_42(__macro) __macro(42)
#define UTIL_REPEAT_IMPL_44(__macro) UTIL_REPEAT_IMPL_43(__macro) __macro(43)
#define UTIL_REPEAT_IMPL_45(__macro) UTIL_REPEAT_IMPL_44(__macro) __macro(44)
#define UTIL_REPEAT_IMPL_46(__macro) UTIL_REPEAT_IMPL_45(__macro) __macro(45)
#define UTIL_REPEAT_IMPL_47(__macro) UTIL_REPEAT_IMPL_46(__macro) __macro(46)
#define UTIL_REPEAT_IMPL_48(__macro) UTIL_REPEAT_IMPL_47(__macro) __macro(47)
#define UTIL_REPEAT_IMPL_49(__macro) UTIL_REPEAT_IMPL_48(__macro) __macro(48)
#define UTIL_REPEAT_IMPL_50(__macro) UTIL_REPEAT_IMPL_49(__macro) __macro(49)
#define UTIL_REPEAT_IMPL_51(__macro) UTIL_REPEAT_IMPL_50(__macro) __macro(50)
#define UTIL_REPEAT_IMPL_52(__macro) UTIL_REPEAT_IMPL_51(__macro) __macro(51)
#define UTIL_REPEAT_IMPL_53(__macro) UTIL_REPEAT_IMPL_52(__macro) __macro(52)
#define UTIL_REPEAT_IMPL_54(__macro) UTIL_REPEAT_IMPL_53(__macro) __macro(53)
#define UTIL_REPEAT_IMPL_55(__macro) UTIL_REPEAT_IMPL_54(__macro) __macro(54)
#define UTIL_REPEAT_IMPL_56(__macro) UTIL_REPEAT_IMPL_55(__macro) __macro(55)
#define UTIL_REPEAT_IMPL_57(__macro) UTIL_REPEAT_IMPL_56(__macro) __macro(56)
#define UTIL_REPEAT_IMPL_58(__macro) UTIL_REPEAT_IMPL_57(__macro) __macro(57)
#define UTIL_REPEAT_IMPL_59(__macro) UTIL_REPEAT_IMPL_58(__macro) __macro(58)
#define UTIL_REPEAT_IMPL_60(__macro) UTIL_REPEAT_IMPL_59(__macro) __macro(59)
#define UTIL_REPEAT_IMPL_61(__macro) UTIL_REPEAT_IMPL_60(__macro) __macro(60)
#define UTIL_REPEAT_IMPL_62(__macro) UTIL_REPEAT_IMPL_61(__macro) __macro(61)
#define UTIL_REPEAT_IMPL_63(__macro) UTIL_REPEAT_IMPL_62(__macro) __macro(62)
#define UTIL_REPEAT_IMPL_64(__macro) UTIL_REPEAT_IMPL_63(__macro) __macro(63)

/**
 * @brief Macro for repeatedly calling __macro __count times
 *
 * Example:
 * @code{.c}
 *   #define PRINT(__i) printf("%d ", __i);
 *   UTIL_REPEAT(3, PRINT);
 * @endcode
 *
 * @param __macro Macro that will receive current number (0;__count)
 */
#define UTIL_REPEAT(__count, __macro) \
  UTIL_CAT(UTIL_REPEAT_IMPL_, __count)(__macro)


/**
 * Code for generating the UTIL_REDUCE_* macros below:
 * @code{.py}
print("#define UTIL_REDUCE_IMPL_0(__op, ...)")
print("#define UTIL_REDUCE_IMPL_1(__op, __x, ...) __x")
for i in range(2, 65):
    print(f"#define UTIL_REDUCE_IMPL_{i}(__op, __x, ...) "
          f"__op(__x, UTIL_REDUCE_IMPL_{i-1}(__op, __VA_ARGS__))")
 * @endcode
 */

#define UTIL_REDUCE_IMPL_0(__op, ...)
#define UTIL_REDUCE_IMPL_1(__op, __x, ...) __x
#define UTIL_REDUCE_IMPL_2(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_1(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_3(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_2(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_4(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_3(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_5(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_4(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_6(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_5(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_7(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_6(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_8(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_7(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_9(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_8(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_10(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_9(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_11(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_10(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_12(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_11(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_13(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_12(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_14(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_13(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_15(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_14(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_16(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_15(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_17(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_16(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_18(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_17(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_19(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_18(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_20(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_19(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_21(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_20(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_22(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_21(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_23(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_22(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_24(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_23(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_25(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_24(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_26(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_25(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_27(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_26(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_28(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_27(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_29(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_28(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_30(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_29(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_31(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_30(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_32(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_31(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_33(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_32(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_34(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_33(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_35(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_34(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_36(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_35(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_37(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_36(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_38(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_37(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_39(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_38(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_40(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_39(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_41(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_40(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_42(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_41(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_43(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_42(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_44(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_43(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_45(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_44(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_46(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_45(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_47(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_46(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_48(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_47(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_49(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_48(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_50(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_49(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_51(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_50(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_52(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_51(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_53(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_52(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_54(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_53(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_55(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_54(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_56(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_55(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_57(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_56(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_58(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_57(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_59(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_58(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_60(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_59(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_61(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_60(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_62(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_61(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_63(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_62(__op, __VA_ARGS__))
#define UTIL_REDUCE_IMPL_64(__op, __x, ...) __op(__x, UTIL_REDUCE_IMPL_63(__op, __VA_ARGS__))

/**
 * @brief Dispatchers to evaluate the argument count before concatenation
 */
#define UTIL_REDUCE_DISPATCH_2(__count, __op, ...) \
    UTIL_CAT(UTIL_REDUCE_IMPL_, __count)(__op, __VA_ARGS__)

#define UTIL_REDUCE_DISPATCH_1(__count, __op, ...) \
    UTIL_REDUCE_DISPATCH_2(__count, __op, __VA_ARGS__)

/**
 * @brief Reduces a variadic list using a binary macro operation (right-fold).
 *
 * Example:
 * @code{.c}
 *   UTIL_REDUCE(UTIL_CAT, a, b, c) -> UTIL_CAT(a, UTIL_CAT(b, c))
 * @endcode
 */
#define UTIL_REDUCE(__op, ...) \
    UTIL_EXPAND(UTIL_REDUCE_DISPATCH_1(UTIL_VA_ARGS_COUNT(__VA_ARGS__), __op, __VA_ARGS__))

