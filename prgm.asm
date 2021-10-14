	.org 0x400

	brk	; 00
	nop	; ea

	.byte 0xff	; ff

	; IRQ VECTOR
	.org 0x500

	plp
	rts	; 60
