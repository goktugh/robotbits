/*
 * This is a replacement for the standard startup routine
 * and saves a significant amount of flash by excluding the vector
 * table, which is unneccessary as we don't use interrupts.
 */

#include <stdint.h>
#include <avr/io.h>
#include <string.h>

extern void main();

__attribute__((naked)) __attribute__ ((used)) __attribute__((section(".ctors"))) 
void startup(void)
{    
    // This register is always assumed to be zero:
    asm volatile("ldi r17,0");
    // Initialise stack pointer
    uint16_t * stackpointer_reg = (uint16_t *) 0x3d;
    *stackpointer_reg = RAMEND;
    // Initialise status register.
    uint8_t * status_reg = (uint8_t *) 0x3f;
    *status_reg = 0;
    // Clear ram
    memset((uint8_t *) RAMSTART, 0, (RAMEND - RAMSTART));
    
    // call main
    main();
}
    
