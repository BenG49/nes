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
	.byte 0xFF