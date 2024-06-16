
/* kernel.h */

#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

typedef struct OSThread {
    uint32_t *sp; // Stack pointer
} OSThread;

void OS_init(void);
void OS_sched(void);
void OS_thread_start(OSThread *me, void (*thread)(void),
                     uint32_t *stkSto, uint32_t stkSize);

#endif /* MIROS_H */
