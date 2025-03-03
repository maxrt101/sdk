/** ========================================================================= *
 *
 * @file scheduler.c
 * @date 14-09-2024
 * @author Maksym Tkachuk <max.r.tkachuk@gmail.com>
 *
 * @brief Preemptive task scheduler
 *
 *  ========================================================================= */

/* Includes ================================================================= */
#include "os/task/scheduler.h"
#include "log/log.h"
#include <stdbool.h>

/* Defines ================================================================== */
/* Macros =================================================================== */
/* Exposed macros =========================================================== */
/* Enums ==================================================================== */
/* Types ==================================================================== */
/* Variables ================================================================ */
/** Scheduler activity flag */
volatile uint8_t os_scheduler_started = false;

/** Pointer to current running task context */
volatile os_task_t * os_task_current;

/** Pointer to list head of tasks */
volatile os_task_t * os_task_head;

/* Private functions ======================================================== */
/* Shared functions ========================================================= */
__NAKED void os_launch(void) {
  asm volatile (
    // Disable interrupts
    "cpsid    i                         \r\n"

    // Load address of scheduler_enabled variable
    "ldr      r0, =os_scheduler_started \r\n"

    // Set 1 to scheduler_enabled
    "movs     r1, #1                    \r\n"
    "str      r1, [r0]                  \r\n"

    // Load tcb
    "ldr      r0, =os_task_current      \r\n"

    // Dereference task_current
    "ldr      r0, [r0]                  \r\n"

    // Load SP into r1
    "ldr      r1, [r0]                  \r\n"

    // Load first task context

    // Firstly load SP, and from it pop stack frame
    // First is part of a frame that we constructed (R4-R11)

    // Set sp
    "mov      sp, r1                    \r\n"

    // Load r8-r11
    "pop      {r4-r7}                   \r\n"
    "mov      r8, r4                    \r\n"
    "mov      r9, r5                    \r\n"
    "mov      r10, r6                   \r\n"
    "mov      r11, r7                   \r\n"

    // Load r4-r7
    "pop      {r4-r7}                   \r\n"

    // Next is CPUs IRQ frame (xPSR, PC, LR, R12, R3, R2, R1, R0)
    "pop      {r0-r3}                   \r\n"
    "pop      {r4}                      \r\n"
    "mov      r12, r4                   \r\n"

    // Skip LR
    "add      sp, sp, #4                \r\n"

    // Pop PC into LR
    "pop      {r4}                      \r\n"
    "mov      lr, r4                    \r\n"
    "add      sp, sp, #4                \r\n"

    "cpsie    i                         \r\n"
    "bx       lr                        \r\n"
  );
}

__NAKED void os_schedule_impl(void) {
  /* On IRQ CPU pushes xPSR, PC, LR, R12, R3, R2, R1, R0
   * Our job is to store remaining R4-R11 (+SP), and load next task
   */

  asm volatile (
#if !OS_IS_SCHEDULE_IRQ_NAKED
    // Is IRQ that calls this function is naked, it pushes R4, LR onto the
    // stack (at least under GCC)
    // So pop R4 & LR into R2, R3, as they're not used in scheduler logic
    "pop      {r2, r3}                  \r\n"
#endif

    // Disable interrupts
    "cpsid    i                         \r\n"

    // Save current task context
    // Save r4-r7
    "push     {r4-r7}                   \r\n"

    // Save r8-r11
    "mov      r4, r8                    \r\n"
    "mov      r5, r9                    \r\n"
    "mov      r6, r10                   \r\n"
    "mov      r7, r11                   \r\n"
    "push     {r4-r7}                   \r\n"

    // Load tcb
    "ldr      r0, =os_task_current      \r\n"

    // Dereference task_current
    "ldr      r0, [r0]                  \r\n"

    // Save sp
    "mov      r1, sp                    \r\n"
    // First 4 bytes in tcb is sp, so store it
    "str      r1, [r0]                  \r\n"

    // Switch to next task

    // Load pointer to current task
    "ldr      r0, =os_task_current      \r\n"

    // Dereference task pointer
    "ldr      r0, [r0]                  \r\n"

    // Second 4 bytes is next task pointer
    "adds     r0, r0, #4                \r\n"

    // Dereference task_current->next
    "ldr      r0, [r0]                  \r\n"

    // If next task is NULL
    "cmp      r0, #0                    \r\n"
    "bne      next_task_ok              \r\n"

    // Set next task to head
    "ldr      r0, =os_task_head         \r\n"

    // Dereference task_head
    "ldr      r0, [r0]                  \r\n"

    "next_task_ok:                      \r\n"

    // Save next task ptr to task_current
    "ldr      r1, =os_task_current      \r\n"
    "str      r0, [r1]                  \r\n"

    // Dereference task pointer
    "ldr      r1, [r1]                  \r\n"

    // Load next task sp into r1
    "ldr      r1, [r0]                  \r\n"

    // Load next task context
    // Set new tasks sp
    "mov      sp, r1                    \r\n"

    // Load r8-r11
    "pop      {r4-r7}                   \r\n"
    "mov      r8, r4                    \r\n"
    "mov      r9, r5                    \r\n"
    "mov      r10, r6                   \r\n"
    "mov      r11, r7                   \r\n"

    // Load r4-r7
    "pop      {r4-r7}                   \r\n"

    // Enable interrupts
    "cpsie    i                         \r\n"

#if !OS_IS_SCHEDULE_IRQ_NAKED
    // Restore saved LR, (r4 that was pushed is discarded)
    "mov      lr, r3                    \r\n"
#endif

    // Branch out from IRQ handler
    "bx       lr                        \r\n"
  );
}

void os_task_exited(void) {
  log_error("Task exited");
  // TODO: Remove exited task from list (should be in os_task_current)
}
