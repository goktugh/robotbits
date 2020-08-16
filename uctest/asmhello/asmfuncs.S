#include <avr/io.h>

; This is testing if we can call
; assembly language functions from C

.global asmtest1

asmtest1:
    ret

.global asmsum

; Define the vport to use to pulse sck line
vport_sck=VPORTA_OUT
bit_sck=7

.macro pulse_sck
    sbi vport_sck, bit_sck 
    nop
    cbi vport_sck, bit_sck 
.endm

; Add two 16-bit integers.
asmsum:
    ; First paramter is r24:r25
    ; Second param is r22:r23
    ; it is little-endian
    ; return value goes in r24:r25
    add r24, r22 ; Low byte
    adc r25, r23 ; High byte with carry from low
    ret ; return value in r24:r25
    
    
; Junk test function test assembler out
junk0:
    pulse_sck
.rept 10 ; Example loop unrolling
    nop
    lsl r24
.endr
    ret
    
    
; Data section variables
; (intialised)
.data

nicebyte1:
    .byte 0
nicebyte2:
    .byte 0
    
; uninitialised vars
.section .bss
nicevar1:
    .space 1
nicevar2:
    .space 1
    
