/** ========================================================================= *
 *
 * @file nvm.h
 * @date 09-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Generic NVM HAL API, has no default implementation
 *
 * Board must implement get_page_size/erase_page/erase/write
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include <stdint.h>
#include <stddef.h>
#include "hal/gpio/gpio.h"
#include "lib/error/error.h"
#include "util/compiler.h"

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Return page size for specific board
 *
 * @retval NVM page size
 */
uint32_t nvm_get_page_size(void);

/**
 * Erase 1 page
 *
 * @param addr Address of page start (if addr is not aligned to page size,
 *             it will be rounded up)
 */
error_t nvm_erase_page(uint32_t addr);

/**
 * Erase multiple pages
 *
 * @param addr Address of page start (if addr is not aligned to page size,
 *             it will be rounded down)
 * @param size Size to erase (will be rounded up to page size)
 */
error_t nvm_erase(uint32_t addr, uint32_t size);

/**
 * Write multiple pages
 *
 * @param addr Address of page start (if addr is not aligned to page size,
 *             it will be rounded down)
 * @param buffer Buffer to write
 * @param size Size to write (will be rounded up to page size)
 */
error_t nvm_write(uint32_t addr, uint8_t * buffer, uint32_t size);

#ifdef __cplusplus
}
#endif