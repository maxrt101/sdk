/** ========================================================================= *
 *
 * @file endianness.h
 * @date 16-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Endianness helper macros and functions
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "util/compiler.h"
#include "util/util.h"
#include <stdint.h>

/* Defines ================================================================== */
/**
 * Detect Endianness
 *
 * IS_LITTLE_ENDIAN is 1 if byte order is little endian, 0 otherwise
 * IS_BIG_ENDIAN is 1 if byte order is big endian, 0 otherwise
 *
 */
#if (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__))
  #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    #define IS_LITTLE_ENDIAN 1
    #define IS_BIG_ENDIAN    0
  #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define IS_LITTLE_ENDIAN 0
    #define IS_BIG_ENDIAN    1
  #else
    #error "Unsupported endiannes"
  #endif
#elif defined(__little_endian__) || (__LITTLE_ENDIAN__ == 1)
  #define IS_LITTLE_ENDIAN 1
  #define IS_BIG_ENDIAN    0
#elif defined(__big_endian__) || (__BIG_ENDIAN__ == 1)
  #define IS_LITTLE_ENDIAN 0
  #define IS_BIG_ENDIAN    1
#else
  #warning "Undefined endiannes"
#endif

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/**
 * Struct for accessing individual bytes in uint16_t
 */
typedef union {
  uint16_t _u16;
  uint8_t  _u8[2];
} u16_buffer_t;

/**
 * Struct for accessing individual bytes in uint32_t
 */
typedef union {
  uint32_t _u32;
  uint16_t _u16[2];
  uint8_t  _u8[4];
} u32_buffer_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Unconditionally swaps bytes in uint16_t
 *
 * @example 0x1234 becomes 0x3412
 *
 * @param value 16 bit value to be swapped
 *
 * @retval Swapped 16 bit value
 */
__STATIC_INLINE uint16_t endian_swap_u16(uint16_t value) {
  return ((value & 0xff00) >> 8) | ((value & 0xff) << 8);
}

/**
 * Unconditionally swaps bytes in uint32_t
 *
 * @example 0x12345678 becomes 0x78563412
 *
 * @param value 32 bit value to be swapped
 *
 * @retval Swapped 32 bit value
 */
__STATIC_INLINE uint32_t endian_swap_u32(uint32_t value) {
  return  ((value >> 24) & 0xff) |      // 3 -> 0
          ((value << 8)  & 0xff0000) |  // 1 -> 2
          ((value >> 8)  & 0xff00) |    // 2 -> 1
          ((value << 24) & 0xff000000); // 0 -> 3
}

/**
 * Converts 16 bit integer to big endian
 *
 * @note On systems, where byte order is big endian does nothing
 */
__STATIC_INLINE uint16_t endian_to_big_u16(uint16_t value) {
  UTIL_IF_1(IS_BIG_ENDIAN,    return value);
  UTIL_IF_1(IS_LITTLE_ENDIAN, return endian_swap_u16(value));
}

/**
 * Converts 16 bit integer to little endian
 *
 * @note On systems, where byte order is little endian does nothing
 */
__STATIC_INLINE uint16_t endian_to_little_u16(uint16_t value) {
  UTIL_IF_1(IS_BIG_ENDIAN,    return endian_swap_u16(value));
  UTIL_IF_1(IS_LITTLE_ENDIAN, return value);
}

/**
 * Converts 16 bit big endian value to system endianness
 *
 * @note On systems, where byte order is big endian does nothing
 */
__STATIC_INLINE uint16_t endian_from_big_u16(uint16_t value) {
  UTIL_IF_1(IS_BIG_ENDIAN,    return value);
  UTIL_IF_1(IS_LITTLE_ENDIAN, return endian_swap_u16(value));
}

/**
 * Converts 16 bit little endian value to system endianness
 *
 * @note On systems, where byte order is little endian does nothing
 */
__STATIC_INLINE uint16_t endian_from_little_u16(uint16_t value) {
  UTIL_IF_1(IS_BIG_ENDIAN,    return endian_swap_u16(value));
  UTIL_IF_1(IS_LITTLE_ENDIAN, return value);
}

/**
 * Converts 32 bit integer to big endian
 *
 * @note On systems, where byte order is big endian does nothing
 */
__STATIC_INLINE uint32_t endian_to_big_u32(uint32_t value) {
  UTIL_IF_1(IS_BIG_ENDIAN,    return value);
  UTIL_IF_1(IS_LITTLE_ENDIAN, return endian_swap_u32(value));
}

/**
 * Converts 32 bit integer to little endian
 *
 * @note On systems, where byte order is little endian does nothing
 */
__STATIC_INLINE uint32_t endian_to_little_u32(uint32_t value) {
  UTIL_IF_1(IS_BIG_ENDIAN,    return endian_swap_u32(value));
  UTIL_IF_1(IS_LITTLE_ENDIAN, return value);
}

/**
 * Converts 32 bit big endian value to system endianness
 *
 * @note On systems, where byte order is big endian does nothing
 */
__STATIC_INLINE uint32_t endian_from_big_u32(uint32_t value) {
  UTIL_IF_1(IS_BIG_ENDIAN,    return value);
  UTIL_IF_1(IS_LITTLE_ENDIAN, return endian_swap_u32(value));
}

/**
 * Converts 32 bit little endian value to system endianness
 *
 * @note On systems, where byte order is little endian does nothing
 */
__STATIC_INLINE uint32_t endian_from_little_u32(uint32_t value) {
  UTIL_IF_1(IS_BIG_ENDIAN,    return endian_swap_u32(value));
  UTIL_IF_1(IS_LITTLE_ENDIAN, return value);
}

#ifdef __cplusplus
}
#endif