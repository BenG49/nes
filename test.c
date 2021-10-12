#include <stdint.h>
#include <stdio.h>

uint16_t mul(uint8_t a, uint8_t b)
{
	uint8_t out_l = 0, out_h = 0;

	for (int i = 0; i < 8; ++i)
	{
		if (b & 1)
			out_h += a;
		b >>= 1;

		uint8_t l = out_h & 1;
		out_h >>= 1;
		out_l >>= 1;
		out_l |= (l << 7);
	}

	return (out_h << 8) + out_l;
}

/*

        10101
    _________
101 ) 1101101
     -101
     ----
        11
        -0
        --
        111
       -101
       ----
         100
          -0
         ---
         1001
         -101
         ----
          100

        LDA #0      ;Initialize REM to 0
        STA REM
        STA REM+1
        LDX #16     ;There are 16 bits in NUM1
L1      ASL NUM1    ;Shift hi bit of NUM1 into REM
        ROL NUM1+1  ;(vacating the lo bit, which will be used for the quotient)
        ROL REM
        ROL REM+1
        LDA REM
        SEC         ;Trial subtraction
        SBC NUM2
        TAY
        LDA REM+1
        SBC NUM2+1
        BCC L2      ;Did subtraction succeed?
        STA REM+1   ;If yes, save it
        STY REM
        INC NUM1    ;and record a 1 in the quotient
L2      DEX
        BNE L1

*/

uint16_t div(uint16_t a, uint16_t b)
{
	int i;
	for (i = 0; a >= b; a -= b, ++i);

	return i;
}

int main()
{
	printf("%d\n", mul(17, 16));
	printf("%d\n", div(20, 20));
}
