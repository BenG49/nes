#include <cpu.hpp>

#include <iostream>

void CPU::set_flags(uint16_t val, Flags to_set)
{
	//         carry
	//		   | neg
	//         | |
	// 0000 0000 0000 0000
	if (to_set.neg)
		f.neg = GET_BIT(val, 7);
	if (to_set.zero)
		f.zero = !LSB(val);
	if (to_set.carry)
		f.carry = GET_BIT(val, 8);
	if (to_set.intr)
	{
		printf("Unimplemented intterupt disable flag");
		exit(1);
	}
}
