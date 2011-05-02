	.file	"main.c"
__SREG__ = 0x3f
__SP_H__ = 0x3e
__SP_L__ = 0x3d
__CCP__  = 0x34
__tmp_reg__ = 0
__zero_reg__ = 1
	.text
.global	flashLED
	.type	flashLED, @function
flashLED:
/* prologue: function */
/* frame size = 0 */
	mov r18,r24
	in r24,55-32
	or r24,r18
	out 55-32,r24
	in r24,56-32
	or r24,r18
	out 56-32,r24
	ldi r24,lo8(2500)
	ldi r25,hi8(2500)
	ldi r20,lo8(200)
	ldi r21,hi8(200)
.L2:
	movw r30,r20
/* #APP */
 ;  105 "c:/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r30,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	sbiw r24,1
	brne .L2
	in r25,56-32
	ldi r24,lo8(0)
	tst r18
	brne .L3
	ldi r24,lo8(1)
.L3:
	and r24,r25
	out 56-32,r24
/* epilogue start */
	ret
	.size	flashLED, .-flashLED
.global	main
	.type	main, @function
main:
	push r28
	push r29
/* prologue: function */
/* frame size = 0 */
	rcall i2c_init
	ldi r24,lo8(8)
	rcall flashLED
	ldi r24,lo8(-92)
	rcall i2c_start
	tst r24
	breq .L7
	rcall i2c_stop
	ldi r28,lo8(200)
	ldi r29,hi8(200)
.L9:
	ldi r24,lo8(8)
	rcall flashLED
	ldi r24,lo8(500)
	ldi r25,hi8(500)
.L8:
	movw r30,r28
/* #APP */
 ;  105 "c:/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r30,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	sbiw r24,1
	brne .L8
	rjmp .L9
.L7:
	ldi r24,lo8(72)
	rcall i2c_write
	ldi r24,lo8(105)
	rcall i2c_write
	rcall i2c_stop
	ldi r28,lo8(200)
	ldi r29,hi8(200)
.L11:
	ldi r24,lo8(8)
	rcall flashLED
	ldi r24,lo8(2500)
	ldi r25,hi8(2500)
.L10:
	movw r30,r28
/* #APP */
 ;  105 "c:/winavr-20100110/lib/gcc/../../avr/include/util/delay_basic.h" 1
	1: sbiw r30,1
	brne 1b
 ;  0 "" 2
/* #NOAPP */
	sbiw r24,1
	brne .L10
	rjmp .L11
	.size	main, .-main
