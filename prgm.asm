	; 10a2
	ldx #$10
	; 00a9
	lda #$0
loop:
	; 0086
	stx $0
	; 0065
	adc $0
	; ca
	dex
	; d0 00f9
	bne loop
