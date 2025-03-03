/** ========================================================================= *
 *
 * @file irq.h
 * @date 19-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief API for turning IRQs on/off
 *
 *  ========================================================================= */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ================================================================= */
#include "error/error.h"
#include "util/compiler.h"

/* Defines ================================================================== */
#define OS_IRQ_ALL 0xFF

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Shared functions ========================================================= */
/**
 * Enables all IRQs
 */
void os_irq_enable(uint8_t irq);

/**
 * Disables all IRQs
 */
void os_irq_disable(uint8_t irq);

/**
 * Sets priority to IRQ
 */
void os_irq_set_prio(uint8_t irq, uint8_t prio);

/**
 * Triggers specific IRQ
 */
void os_irq_trigger(uint8_t irq);

/**
 * Port of os_irq_enable, BSP defined
 *
 * Not required, has a stub implementation
 */
void os_irq_enable_port(uint8_t irq);

/**
 * Port of os_irq_disable, BSP defined
 *
 * Not required, has a stub implementation
 */
void os_irq_disable_port(uint8_t irq);

/**
 * Port of os_irq_set_prio, BSP defined
 *
 * Not required, has a stub implementation
 */
void os_irq_set_prio_port(uint8_t irq, uint8_t prio);

/**
 * Port of os_irq_trigger, BSP defined
 *
 * Not required, has a stub implementation
 */
void os_irq_trigger_port(uint8_t irq);

#ifdef __cplusplus
}
#endif