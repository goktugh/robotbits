
obj_attiny1614/main.elf:     file format elf32-avr


Disassembly of section .text:

00000000 <__vectors>:
   0:	0c 94 3e 00 	jmp	0x7c	; 0x7c <__ctors_end>
   4:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
   8:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
   c:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  10:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  14:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  18:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  1c:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  20:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  24:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  28:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  2c:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  30:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  34:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  38:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  3c:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  40:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  44:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  48:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  4c:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  50:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  54:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  58:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  5c:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  60:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  64:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  68:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  6c:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  70:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  74:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>
  78:	0c 94 48 00 	jmp	0x90	; 0x90 <__bad_interrupt>

0000007c <__ctors_end>:
  7c:	11 24       	eor	r1, r1
  7e:	1f be       	out	0x3f, r1	; 63
  80:	cf ef       	ldi	r28, 0xFF	; 255
  82:	cd bf       	out	0x3d, r28	; 61
  84:	df e3       	ldi	r29, 0x3F	; 63
  86:	de bf       	out	0x3e, r29	; 62
  88:	0e 94 56 00 	call	0xac	; 0xac <main>
  8c:	0c 94 82 00 	jmp	0x104	; 0x104 <_exit>

00000090 <__bad_interrupt>:
  90:	0c 94 00 00 	jmp	0	; 0x0 <__vectors>

00000094 <diag_puts>:
  94:	fc 01       	movw	r30, r24
  96:	81 91       	ld	r24, Z+
  98:	81 11       	cpse	r24, r1
  9a:	01 c0       	rjmp	.+2      	; 0x9e <diag_puts+0xa>
  9c:	08 95       	ret
  9e:	90 91 04 08 	lds	r25, 0x0804	; 0x800804 <__TEXT_REGION_LENGTH__+0x700804>
  a2:	95 ff       	sbrs	r25, 5
  a4:	fc cf       	rjmp	.-8      	; 0x9e <diag_puts+0xa>
  a6:	80 93 02 08 	sts	0x0802, r24	; 0x800802 <__TEXT_REGION_LENGTH__+0x700802>
  aa:	f5 cf       	rjmp	.-22     	; 0x96 <diag_puts+0x2>

000000ac <main>:
  ac:	88 ed       	ldi	r24, 0xD8	; 216
  ae:	90 e0       	ldi	r25, 0x00	; 0
  b0:	84 bf       	out	0x34, r24	; 52
  b2:	90 93 61 00 	sts	0x0061, r25	; 0x800061 <__TEXT_REGION_LENGTH__+0x700061>
  b6:	8f e4       	ldi	r24, 0x4F	; 79
  b8:	93 ec       	ldi	r25, 0xC3	; 195
  ba:	01 97       	sbiw	r24, 0x01	; 1
  bc:	f1 f7       	brne	.-4      	; 0xba <main+0xe>
  be:	00 c0       	rjmp	.+0      	; 0xc0 <main+0x14>
  c0:	00 00       	nop
  c2:	81 e0       	ldi	r24, 0x01	; 1
  c4:	80 93 01 02 	sts	0x0201, r24	; 0x800201 <__TEXT_REGION_LENGTH__+0x700201>
  c8:	82 e0       	ldi	r24, 0x02	; 2
  ca:	80 93 05 04 	sts	0x0405, r24	; 0x800405 <__TEXT_REGION_LENGTH__+0x700405>
  ce:	80 93 01 04 	sts	0x0401, r24	; 0x800401 <__TEXT_REGION_LENGTH__+0x700401>
  d2:	8b e5       	ldi	r24, 0x5B	; 91
  d4:	91 e0       	ldi	r25, 0x01	; 1
  d6:	80 93 08 08 	sts	0x0808, r24	; 0x800808 <__TEXT_REGION_LENGTH__+0x700808>
  da:	90 93 09 08 	sts	0x0809, r25	; 0x800809 <__TEXT_REGION_LENGTH__+0x700809>
  de:	80 e4       	ldi	r24, 0x40	; 64
  e0:	80 93 06 08 	sts	0x0806, r24	; 0x800806 <__TEXT_REGION_LENGTH__+0x700806>
  e4:	88 e0       	ldi	r24, 0x08	; 8
  e6:	91 e8       	ldi	r25, 0x81	; 129
  e8:	0e 94 4a 00 	call	0x94	; 0x94 <diag_puts>
  ec:	9f e7       	ldi	r25, 0x7F	; 127
  ee:	24 e8       	ldi	r18, 0x84	; 132
  f0:	8e e1       	ldi	r24, 0x1E	; 30
  f2:	91 50       	subi	r25, 0x01	; 1
  f4:	20 40       	sbci	r18, 0x00	; 0
  f6:	80 40       	sbci	r24, 0x00	; 0
  f8:	e1 f7       	brne	.-8      	; 0xf2 <main+0x46>
  fa:	00 c0       	rjmp	.+0      	; 0xfc <main+0x50>
  fc:	00 00       	nop
  fe:	85 e2       	ldi	r24, 0x25	; 37
 100:	91 e8       	ldi	r25, 0x81	; 129
 102:	f2 cf       	rjmp	.-28     	; 0xe8 <main+0x3c>

00000104 <_exit>:
 104:	f8 94       	cli

00000106 <__stop_program>:
 106:	ff cf       	rjmp	.-2      	; 0x106 <__stop_program>
