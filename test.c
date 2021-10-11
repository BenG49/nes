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

int main()
{
	printf("%d\n", mul(17, 16));
}
