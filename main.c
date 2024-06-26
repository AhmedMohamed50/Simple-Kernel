
/* Application code */

#include <stdint.h>
#include "kernel.h"
#include "bsp.h"

uint32_t stack_blinky1[40];
OSThread blinky1;
void main_blinky1() {
    while (1) {
        BSP_ledRedOn();
        BSP_delay(BSP_TICKS_PER_SEC / 4U);
        BSP_ledRedOff();
        BSP_delay(BSP_TICKS_PER_SEC * 3U / 4U);
    }
}

uint32_t stack_blinky2[40];
OSThread blinky2;
void main_blinky2() {
    while (1) {
        BSP_ledBlueOn();
        BSP_delay(BSP_TICKS_PER_SEC / 2U);
        BSP_ledBlueOff();
        BSP_delay(BSP_TICKS_PER_SEC / 3U);
    }
}

int main(void) {
    BSP_init();
    OS_init();

    /* fabricate Cortex-M ISR stack frame for blinky1 */
    OS_thread_start(&blinky1,
                    &main_blinky1,
                    stack_blinky1, sizeof(stack_blinky1));

    /* fabricate Cortex-M ISR stack frame for blinky2 */
    OS_thread_start(&blinky2,
                    &main_blinky2,
                    stack_blinky2, sizeof(stack_blinky2));

    //BSP_ledRedOn();
    while (1) {
        OS_sched(); /* schedule threads in the main loop */
    }

    //return 0;
}

