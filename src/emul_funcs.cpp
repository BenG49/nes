#include <emul.hpp>

void Emul::add(cpu_t n)
{
	uint16_t out = (uint16_t)cpu->a + (uint16_t)n + cpu->f.carry;

	cpu->f.ov = GET_BIT((cpu->a ^ out) & (n ^ out), 7);
	cpu->set_flags(out, Flags(true, true, true));

	cpu->a = out;
}

void Emul::and_f(cpu_t n)
{
	cpu->a = cpu->a & n;

	cpu->set_flags(cpu->a, Flags(true, true, false));
}

cpu_t Emul::asl(cpu_t n)
{
	uint16_t out = ((uint16_t)n) << 1;

	cpu->set_flags(out, Flags(true, true, true));

	return out;
}

void Emul::bit(cpu_t n)
{
	// lol i have no idea why this is useful
	cpu->f.neg = GET_BIT(n, 7);
	cpu->f.ov  = GET_BIT(n, 6);

	cpu->f.zero = !(n & cpu->a);
}

void Emul::cmp(cpu_t n)
{
	uint16_t out = (uint16_t)cpu->a - (uint16_t)n;

	cpu->set_flags(out, Flags(true, true, true));
}

void Emul::cpx(cpu_t n)
{
	uint16_t out = (uint16_t)cpu->x - (uint16_t)n;

	cpu->set_flags(out, Flags(true, true, true));
}

void Emul::cpy(cpu_t n)
{
	uint16_t out = (uint16_t)cpu->y - (uint16_t)n;

	cpu->set_flags(out, Flags(true, true, true));
}

void Emul::eor(cpu_t n)
{
	cpu->a = cpu->a ^ n;

	cpu->set_flags(cpu->a, Flags(true, true, false));
}

void Emul::lda(cpu_t n)
{
	cpu->set_flags(cpu->a = n, Flags(true, true, false));
}

void Emul::ldx(cpu_t n)
{
	cpu->set_flags(cpu->x = n, Flags(true, true, false));
}

void Emul::ldy(cpu_t n)
{
	cpu->set_flags(cpu->y = n, Flags(true, true, false));
}

void Emul::ora(cpu_t n)
{
	cpu->a = cpu->a | n;

	cpu->set_flags(cpu->a, Flags(true, true, false));
}

void Emul::sbc(cpu_t n)
{
	uint16_t out = (uint16_t)cpu->a - (uint16_t)n - cpu->f.carry;

	cpu->f.ov = GET_BIT((cpu->a ^ out) & ((255 - n) ^ out), 7);
	cpu->set_flags(out, Flags(true, true, true));

	cpu->a = out;
}

cpu_t Emul::rol(cpu_t n)
{
	uint16_t out = ((uint16_t)n) << 1;

	out |= cpu->f.carry;
	cpu->set_flags(out, Flags(true, true, true));

	return out;
}

cpu_t Emul::ror(cpu_t n)
{
	uint16_t out = ((uint16_t)n) >> 1;

	out |= (cpu->f.carry << 7);
	cpu->f.carry = n & 1;
	cpu->set_flags(out, Flags(true, true, false));

	return out;
}

void Emul::sta(addr_t n)
{
	cpu->mem[n] = cpu->a;
}

void Emul::stx(addr_t n)
{
	printf("stx %d\n", n);
	cpu->mem[n] = cpu->x;
}

void Emul::sty(addr_t n)
{
	cpu->mem[n] = cpu->y;
}

void Emul::push(cpu_t n)
{
	cpu->mem[cpu->sp--] = n;
}

cpu_t Emul::pop()
{
	return cpu->mem[++cpu->sp];
}

addr_t Emul::pop_word()
{
	uint16_t out = 0;
	out |= cpu->mem[++cpu->sp];
	out |= (cpu->mem[++cpu->sp] << 8);
	return out;
}
