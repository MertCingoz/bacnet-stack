/**
 * @brief The module handles CSTACK monitoring
 * @author Steve Karg <skarg@users.sourceforge.net>
 * @date 2009
 * @copyright SPDX-License-Identifier: MIT
 */
#include <stdint.h>
#include "hardware.h"
#include "stack.h"

/* stack checking */
#if defined(__GNUC__)
extern uint8_t _end;
extern uint8_t __stack;
#endif

#if defined(__GNUC__)
#define STACK_CANARY (0xC5)
void stack_init(void) __attribute__((naked)) __attribute__((section(".init1")));
#endif

void stack_init(void)
{
#if defined(__GNUC__)
#if 0
    uint8_t *p = &_end;

    while (p <= &__stack) {
        *p = STACK_CANARY;
        p++;
    }
#else
    __asm volatile("    ldi r30,lo8(_end)\n"
                   "    ldi r31,hi8(_end)\n"
                   "    ldi r24,lo8(0xc5)\n" /* STACK_CANARY = 0xc5 */
                   "    ldi r25,hi8(__stack)\n"
                   "    rjmp .cmp\n"
                   ".loop:\n"
                   "    st Z+,r24\n"
                   ".cmp:\n"
                   "    cpi r30,lo8(__stack)\n"
                   "    cpc r31,r25\n"
                   "    brlo .loop\n"
                   "    breq .loop" ::);
#endif
#endif
}

unsigned stack_size(void)
{
#if defined(__GNUC__)
    return (&__stack) - (&_end);
#else
    return 0;
#endif
}

uint8_t stack_byte(unsigned offset)
{
#if defined(__GNUC__)
    return *(&_end + offset);
#else
    offset = offset;
    return 0;
#endif
}

unsigned stack_unused(void)
{
    unsigned count = 0;
#if defined(__GNUC__)
    uint8_t *p = &_end;

    while (p <= &__stack) {
        if ((*p) != STACK_CANARY) {
            count = p - (&_end);
            break;
        }
        p++;
    }
#endif
    return count;
}
