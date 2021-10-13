	.org 0x400

	jmp L1 ; test JMP
	nop
	nop
L1:	sec
	bcc Error
	bcs L2
	nop
	nop
L2:	clc
	bcs Error 

Error:
	.org 0x400

	; IRQ VECTOR
	.org 0x500
