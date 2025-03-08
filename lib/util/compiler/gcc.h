/** ========================================================================= *
 *
 * @file gcc.h
 * @date 20-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @note Uses cmsis for most macros
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#if defined __has_include
#if __has_include("cmsis_compiler.h")
#include "cmsis_compiler.h"
#endif
#endif

/* Defines ================================================================== */
#ifndef __NAKED
#define __NAKED __attribute__((naked))
#endif

#ifndef __PURE
#define __PURE __attribute__((pure))
#endif

#ifndef __CONST
#define __CONST __attribute__((const))
#endif

#ifndef __RAMFUNC
#define __RAMFUNC __attribute__((noinline, section(".RamFunc")))
#endif

#ifndef __OPTIMIZE
#define __OPTIMIZE(n) __attribute__((optimize(n)))
#endif

#ifndef __UNUSED
#define __UNUSED __attribute__((unused))
#endif

#ifndef __ALIAS
#define __ALIAS(new_name) __attribute__((alias(new_name)))
#endif

#ifndef __ALIGN
#define __ALIGN(n) __attribute__((aligned(n)))
#endif

#ifndef __PACKED_ENUM
#define __PACKED_ENUM enum __PACKED
#endif

#ifndef __LOCATION
#define __LOCATION(address) __attribute__((location(address)))
#endif

#ifndef __SECTION
#define __SECTION(__section) __attribute__((section("." #__section)))
#endif

#ifndef __CONSTRUCTOR
#define __CONSTRUCTOR(priority) __attribute__((constructor(priority)))
#endif

#ifndef __DEPRECATED
#define __DEPRECATED __attribute__((deprecated))
#endif

#ifndef offsetof
#define offsetof(__type, __member) __builtin_offsetof(__type, __member)
#endif

#ifndef __STATIC_ASSERT
#define __STATIC_ASSERT(__expr, __msg) _Static_assert(__expr, __msg)
#endif

#ifndef __NO_INLINE
#define __NO_INLINE __attribute__((noinline))
#endif

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */

#ifdef __cplusplus
}
#endif

