
/* kernel.c */

#include "kernel.h"

#define STACK_FRAME_SIZE 8U
#define NUM_THREADS 2U

static OSThread * volatile OS_curr;
static OSThread * volatile OS_next;

OSThread *OS_thread[NUM_THREADS];
uint32_t OS_thread_count = 0U;
uint32_t OS_thread_index = 0U;

void OS_init(void) {
    /* no initialization needed for this simple kernel */
}

void OS_sched(void) {
    if (OS_thread_count > 0U) {
        OS_thread_index = (OS_thread_index + 1U) % OS_thread_count;
        OS_next = OS_thread[OS_thread_index];
        if (OS_next != OS_curr) {
            *(uint32_t volatile *)0xE000ED04 = (1U << 28); /* trigger PendSV */
        }
    }
}

void OS_thread_start(OSThread *me, void (*thread)(void),
                     uint32_t *stkSto, uint32_t stkSize) {
    uint32_t *sp = &stkSto[stkSize / sizeof(uint32_t)]; /* stack pointer */
    *(--sp) = (1U << 24); /* xPSR (thumb bit) */
    *(--sp) = (uint32_t)thread; /* PC (program counter) */
    *(--sp) = 0x0000000EU; /* LR (link register) */
    *(--sp) = 0x0000000CU; /* R12 */
    *(--sp) = 0x00000003U; /* R3 */
    *(--sp) = 0x00000002U; /* R2 */
    *(--sp) = 0x00000001U; /* R1 */
    *(--sp) = 0x00000000U; /* R0 */
    me->sp = sp; /* save the top of the stack in the thread control block */

    OS_thread[OS_thread_count++] = me;
    if (OS_thread_count == 1U) {
        OS_curr = OS_thread[0];
    }
}

/* PendSV_Handler */
void PendSV_Handler(void) {
    __asm volatile (
        "CPSID I \n" /* disable interrupts */
        "MRS r0, PSP \n" /* get process stack pointer */
        "STMDB r0!, {r4-r11} \n" /* save r4-r11 on process stack */

        "LDR r1, =OS_curr \n"
        "LDR r2, [r1] \n"
        "STR r0, [r2] \n" /* save process stack pointer in OS_curr */

        "LDR r1, =OS_next \n"
        "LDR r2, [r1] \n"
        "LDR r0, [r2] \n" /* load process stack pointer from OS_next */

        "LDMIA r0!, {r4-r11} \n" /* restore r4-r11 from process stack */
        "MSR PSP, r0 \n" /* set process stack pointer */

        "LDR r1, =OS_curr \n"
        "LDR r2, [r1] \n"
        "LDR r1, =OS_next \n"
        "STR r2, [r1] \n" /* OS_curr = OS_next */

        "CPSIE I \n" /* enable interrupts */
        "BX LR \n" /* return from exception */
    );
}

