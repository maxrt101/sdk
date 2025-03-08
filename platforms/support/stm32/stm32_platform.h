/** ========================================================================= *
 *
 * @file stm32_platform.h
 * @date 07-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief STM32 Platform support code
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/assertion.h"
#include "error/error.h"

#if STM32L0xx
#include "stm32l0xx_hal.h"
#elif STM32F1xx
#include "stm32f1xx_hal.h"
#endif

/* Defines ================================================================== */
/* Macros =================================================================== */
/**
 * ERROR_CHECK_RETURN but for HAL_StatusTypeDef
 */
#define STM32_HAL_ERROR_CHECK_RETURN(expr) \
  ERROR_CHECK_RETURN(stm32_hal_status_to_error(expr))

/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Converts HAL_StatusTypeDef to error_t
 */
error_t stm32_hal_status_to_error(HAL_StatusTypeDef hal_status);


#ifdef __cplusplus
}
#endif