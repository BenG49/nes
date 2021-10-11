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

;	lda #2
;	sta $0
;	lda #3
;	sta $1
;	jsr mul
;	jmp end

;; uses 0x10
;mul:
;	lda #0
;	sta $20
;	ldx #8
;mul_loop:
;	lsr $1
;	bcc mul_zero
;	clc
;	adc $0
;mul_zero:
;	ror a
;	ror $20
;	dex
;	bne mul_loop
;	sta $1
;	lda $20
;	sta $0
;	rts

	.org $300

	; 300
	jsr func
	; 303
	jmp end

	; 306
func:
	rts

end:
