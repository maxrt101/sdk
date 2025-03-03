/** ========================================================================= *
 *
 * @file color.h
 * @date 23-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief ANSI terminal colors
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
/* Defines ================================================================== */
#define COLOR_RED       "\033[31m"
#define COLOR_GREEN     "\033[32m"
#define COLOR_YELLOW    "\033[33m"
#define COLOR_BLUE      "\033[34m"
#define COLOR_MAGENTA   "\033[35m"
#define COLOR_CYAN      "\033[36m"
#define COLOR_WHITE     "\033[37m"
#define COLOR_RED_BG    "\033[41m"
#define COLOR_RESET     "\033[0m"

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */

#ifdef __cplusplus
}
#endif