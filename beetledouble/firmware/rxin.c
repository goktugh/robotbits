#include <avr/io.h>

void rxin_init()
{
    // (see datasheet 14.5.3 Asynchronous Channel n Generator Selection)
    // Configure ASYNC event 1 to read PWMIN1 
    // Which will be routed to TCB0
    // From PIN PB3
    EVSYS.ASYNCCH1 = 0x0d; 
    EVSYS.ASYNCUSER0 = 0x4; // Route TCB0 from ASYNCCH1

    // Configure ASYNC event 0 to read PWMIN2 from PA3 
    // Which will be routed to TCB1
    EVSYS.ASYNCCH0 = 0x0d; 
    EVSYS.ASYNCUSER11 = 0x3; // Route to TCB1 from ASYNCCH0
    
    // Configure TCB0 and TCB1 identically, except they will
    // take their input from different pins.
    // We can poll TCBn.INTFLAGS to detect when an input capture
    // is ready, then we read TCBn.CCMP which will auto-clear the
    // INTFLAGS 
    void init_tcb(TCB_t *tcb) {
        // Clock: divide the system clock by 2,
        // So it's counting in 10mhz / 2 200ns ticks
        // So it wraps after (65536 / 5) milliseconds 
        tcb->CTRLA = (0x1 << 1); // CLKSEL CLK_PER/2
        // Set count-mode
        tcb->CTRLB = 0x4; // Pulse-width measurement
        tcb->EVCTRL = 0; // select positive edge clear, negative edge capture
        tcb->CNT = 0;
        tcb->INTFLAGS = 0x1; // clear any existing int
        tcb->CTRLA |= 0x1; // set ENABLE
    }
    
    init_tcb(&TCB0);
    init_tcb(&TCB1);
}

void rxin_loop()
{
    
}
