#include <cpu.hpp>

#include <iostream>

#define BR(flag) \
	if (flag) pc = addr;

#define TR(func, to, from) \
	void CPU::func(uint16_t addr) { to = from; }

// ----------- //

void CPU::set_flags(uint16_t val, bool neg, bool zero, bool carry)
{
	if (neg)
		SET_BIT(sr, GET_BIT(val, 7), NEG);
	if (zero)
		SET_BIT(sr, !LSB(val), ZERO);
	if (carry)
		SET_BIT(sr, GET_BIT(val, 8), CARRY);
}

void CPU::push(uint8_t n) { bus_write(0x100 + sp--, n); }
void CPU::push_word(uint16_t n)
{
	push(MSB(n));
	push(LSB(n));
}

uint8_t CPU::pop() { return bus_read(0x100 + ++sp); }
uint16_t CPU::pop_word()
{
	uint8_t l = pop();
	uint8_t h = pop();
	return (h << 8) | l;
}

// ----------- //

void CPU::adc(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)a + n + GET_BIT(sr, CARRY);

	if (GET_BIT(sr, DEC))
	{
		puts("Unimplemented decimal mode add");
		exit(1);
	}

	SET_BIT(sr, ~(a ^ n) & (a ^ out) & 0x80, OV);
	set_flags(out, true, true, true);

	a = LSB(out);
}

void CPU::AND(uint16_t addr)
{
	uint8_t n = bus_read(addr);
	a = a & n;

	set_flags(a, true, true, false);
}

void CPU::asl(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	uint16_t out = n << 1;

	set_flags(out, true, true, true);

	bus_write(addr, LSB(out));
}

void CPU::asl_acc(uint16_t n) { set_flags(a <<= 1, true, true, true); }

void CPU::bcc(uint16_t addr) { BR(!GET_BIT(sr, CARRY)) }
void CPU::bcs(uint16_t addr) { BR(GET_BIT(sr, CARRY)) }
void CPU::beq(uint16_t addr) { BR(GET_BIT(sr, ZERO)) }

void CPU::bit(uint16_t addr)
{
	uint8_t n = bus_read(addr);
	// lol i have no idea why this is useful
	SET_BIT(sr, GET_BIT(n, 7), NEG);
	SET_BIT(sr, GET_BIT(n, 6), OV);

	SET_BIT(sr, !(n & a), ZERO);
}

void CPU::bmi(uint16_t addr) { BR(GET_BIT(sr, NEG)) }
void CPU::bne(uint16_t addr) { BR(!GET_BIT(sr, ZERO)) }
void CPU::bpl(uint16_t addr) { BR(!GET_BIT(sr, NEG)) }

void CPU::brk(uint16_t addr)
{
	push_word(++pc);
	push(sr | BRK);
	SET_BIT(sr, true, INT);

	pc = (bus_read(IRQL) << 8) | bus_read(IRQH);
}

void CPU::bvc(uint16_t addr) { BR(!GET_BIT(sr, OV)) }
void CPU::bvs(uint16_t addr) { BR(GET_BIT(sr, OV)) }

void CPU::clc(uint16_t addr) { SET_BIT(sr, false, CARRY); }
void CPU::cld(uint16_t addr) { SET_BIT(sr, false, DEC); }
void CPU::cli(uint16_t addr) { SET_BIT(sr, false, INT); }
void CPU::clv(uint16_t addr) { SET_BIT(sr, false, OV); }

void CPU::cmp(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)a - n;

	set_flags(out, true, true, true);
}

void CPU::cpx(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)x - n;

	set_flags(out, true, true, true);
}

void CPU::cpy(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)y - n;

	set_flags(out, true, true, true);
}

void CPU::dec(uint16_t addr)
{
	uint8_t n = bus_read(addr) - 1;

	bus_write(addr, n);
	set_flags(n, true, true, false);
}

void CPU::dex(uint16_t addr) { set_flags(--x, true, true, false); }
void CPU::dey(uint16_t addr) { set_flags(--y, true, true, false); }

void CPU::eor(uint16_t addr)
{
	set_flags(a ^= bus_read(addr), true, true, false);
}

void CPU::inc(uint16_t addr)
{
	uint8_t n = bus_read(addr) + 1;

	bus_write(addr, n);
	set_flags(n, true, true, false);
}

void CPU::inx(uint16_t addr) { set_flags(++x, true, true, false); }
void CPU::iny(uint16_t addr) { set_flags(++y, true, true, false); }

void CPU::jmp(uint16_t addr) { pc = addr; }
// pc is at instruction after jump, address to lsb of addr is pushed
void CPU::jsr(uint16_t addr) { push_word(pc - 1); pc = addr; }

void CPU::lda(uint16_t addr) { set_flags(a = bus_read(addr), true, true, false); }
void CPU::ldx(uint16_t addr) { set_flags(x = bus_read(addr), true, true, false); }
void CPU::ldy(uint16_t addr) { set_flags(y = bus_read(addr), true, true, false); }

void CPU::lsr(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	SET_BIT(sr, n & 1, CARRY);
	SET_BIT(sr, false, NEG);
	uint16_t out = n >> 1;

	set_flags(out, true, true, false);

	bus_write(addr, LSB(out));
}

void CPU::lsr_acc(uint16_t n)
{
	SET_BIT(sr, a & 1, CARRY);
	SET_BIT(sr, false, NEG);

	set_flags(a >>= 1, true, true, false);
}

void CPU::nop(uint16_t addr) {}

void CPU::ora(uint16_t addr) { set_flags(a |= bus_read(addr), true, true, false); }

void CPU::pha(uint16_t addr) { push(a); }
void CPU::php(uint16_t addr) { push(sr); }
void CPU::pla(uint16_t addr) { set_flags(a = pop(), true, true, false); }
void CPU::plp(uint16_t addr) { sr = pop() | CONST; }

void CPU::rol(uint16_t addr)
{
	uint16_t out = (uint16_t)bus_read(addr) << 1;
	out |= GET_BIT(sr, CARRY);

	set_flags(out, true, true, true);

	bus_write(addr, LSB(out));
}

void CPU::rol_acc(uint16_t n) { set_flags((a << 1) | GET_BIT(sr, CARRY), true, true, true); }

void CPU::ror(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	SET_BIT(sr, n & 1, CARRY);
	uint16_t out = n >> 1;
	SET_BIT(out, GET_BIT(sr, CARRY), 7);

	set_flags(out, true, true, false);

	bus_write(addr, LSB(out));
}

void CPU::ror_acc(uint16_t n)
{
	SET_BIT(sr, a & 1, CARRY);
	a >>= 1;
	SET_BIT(a, GET_BIT(sr, CARRY), 7);
	set_flags(a, true, true, false);
}

void CPU::rti(uint16_t addr) { sr = pop(); pc = pop_word(); }
void CPU::rts(uint16_t addr) { pc = pop_word() + 1; }

void CPU::sbc(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)a - n - GET_BIT(sr, CARRY);

	if (GET_BIT(sr, DEC))
	{
		puts("Unimplemented decimal mode sub");
		exit(1);
	}

	SET_BIT(sr, ((a ^ out) & 0x80) & ((a ^ n) & 0x80), OV);
	set_flags(out, true, true, true);

	a = LSB(out);
}

void CPU::sec(uint16_t addr) { SET_BIT(sr, true, CARRY); }
void CPU::sed(uint16_t addr) { SET_BIT(sr, true, DEC); }
void CPU::sei(uint16_t addr) { SET_BIT(sr, true, INT); }

void CPU::sta(uint16_t addr) { bus_write(addr, a); }
void CPU::stx(uint16_t addr) { bus_write(addr, x); }
void CPU::sty(uint16_t addr) { bus_write(addr, y); }

TR(tax, a, x)
TR(tay, a, y)
TR(tsx, sp, x)
TR(txa, x, a)
TR(txs, x, sp)
TR(tya, y, a)
