	.org 0x300

	; gpf

;	// Square root of integer
;	unsigned long int_sqrt ( unsigned long s )
;	{
;		unsigned long x0 = s >> 1;				// Initial estimate
;	                                            // Avoid overflow when s is the maximum representable value
;	
;		// Sanity check
;		if ( x0 )
;		{
;			unsigned long x1 = ( x0 + s / x0 ) >> 1;	// Update
;			
;			while ( x1 < x0 )				// This also checks for cycle
;			{
;				x0 = x1;
;				x1 = ( x0 + s / x0 ) >> 1;
;			}
;			
;			return x0;
;		}
;		else
;		{
;			return s;
;		}
;	}

	lda #20
	sta $0
	lda #2
	sta $2
	jsr div
	jmp end

; passed in $0 and $1
; uses 0x10
mul:
	lda #0
	sta $10
	ldx #8
mul_loop:
	lsr $1
	bcc mul_zero
	clc
	adc $0
mul_zero:
	ror a
	ror $10
	dex
	bne mul_loop
	sta $1
	lda $10
	sta $0
	rts

; pass u16 in $0-$1, pass u16 in $2-$3
; uses $10-$11
; return in $0-$1
div:
	; a >= b
	lda $1
	cmp $3
	bcc div_end
	bne div_cont
	lda $0
	cmp $2
	bcc div_end

div_cont:
	; a -= b
	lda $0
	sbc $2
	sta $0
	lda $1
	sbc $3
	sta $1

	; ++i
	inc $10
	lda $11
	adc #0
	sta $11
	jmp div

div_end:
	lda $10
	sta $0
	lda $11
	sta $1
	rts

end:
	lda $0
	lda $1
