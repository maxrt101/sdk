/** ========================================================================= *
 *
 * @file stm32_platform.c
 * @date 07-03-2025
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief STM32 Platform support code
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "stm32_platform.h"

/* Defines ================================================================== */
#define LOG_TAG stm32

/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
error_t stm32_hal_status_to_error(HAL_StatusTypeDef hal_status) {
  switch (hal_status) {
    case HAL_OK:
      return E_OK;
    case HAL_ERROR:
      return E_FAILED;
    case HAL_BUSY:
      return E_BUSY;
    case HAL_TIMEOUT:
      return E_TIMEOUT;
    default:
      return E_INVAL;
  }
}
