/** ========================================================================= *
 *
 * @file irq.c
 * @date 19-08-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "os/irq/irq.h"
#include "log/log.h"

/* Defines ================================================================== */
#define LOG_TAG IRQ

/* Macros =================================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/* Private functions ======================================================== */
/* Shared functions ========================================================= */
void os_irq_enable(uint8_t irq) {
  os_irq_enable_port(irq);
}

void os_irq_disable(uint8_t irq) {
  os_irq_disable_port(irq);
}

void os_irq_set_prio(uint8_t irq, uint8_t prio) {
  os_irq_set_prio_port(irq, prio);
}

void os_irq_trigger(uint8_t irq) {
  os_irq_trigger_port(irq);
}

__WEAK void os_irq_enable_port(uint8_t irq) {
  log_warn("os_irq_enable is using weak stub");
}

__WEAK void os_irq_disable_port(uint8_t irq) {
  log_warn("os_irq_enable is using weak stub");
}

__WEAK void os_irq_set_prio_port(uint8_t irq, uint8_t prio) {
  log_warn("os_irq_set_prio is using weak stub");
}

__WEAK void os_irq_trigger_port(uint8_t irq) {
  log_warn("os_irq_trigger is using weak stub");
}
