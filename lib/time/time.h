/** ========================================================================= *
 *
 * @file time.h
 * @date 20-07-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdint.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * Converts milliseconds to ticks given ms value and ms per tick
 */
#define MS_TO_TICKS(ms, ms_per_tick) \
    (ms + (ms_per_tick / 2U)) / (ms_per_tick))

/**
 * Converts ticks to milliseconds given ticks value and ms per tick
 */
#define TICKS_TO_MS(ticks, ms_per_tick) \
    ((ticks) * (ms_per_tick))

/* Enums ==================================================================== */
/* Types ==================================================================== */
typedef uint32_t milliseconds_t;

/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Increases global time (in ms)
 * User should call this in systick, or RTC
 */
void runtime_inc(milliseconds_t ms);

/**
 * Gets global time (in ms)
 */
milliseconds_t runtime_get();

/**
 * Sets global time (in ms)
 */
void runtime_set(milliseconds_t ms);

#ifdef __cplusplus
}
#endif

