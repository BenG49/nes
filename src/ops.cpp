#include <cpu.hpp>

#include <iostream>

#define BR(flag)                            \
	if (flag) {                             \
		if (MSB(addr) != MSB(pc)) cycles++; \
                                            \
		pc = addr;                          \
                                            \
		cycles++;                           \
	}

#define TR(func, from, to) \
	void CPU::func(uint16_t addr) { set_flags(to = from, true, true, false); }

// ----------- //

void CPU::set_flags(uint16_t val, bool neg, bool zero, bool carry)
{
	if (neg) sr.neg = GET_BIT(val, 7);
	if (zero) sr.zero = !LSB(val);
	if (carry) sr.carry = GET_BIT(val, 8);
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
	/*if (GET_BIT(sr, DEC))
	{
		puts("Unimplemented decimal mode add");
		exit(1);
	}*/

	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)a + n + sr.carry;

	sr.ov = ~(a ^ n) & (a ^ out) & 0x80;
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

void CPU::asl_acc(uint16_t n)
{
	uint16_t out = (uint16_t)a << 1;

	a = out & 0xff;
	
	set_flags(out, true, true, true);
}

void CPU::bcc(uint16_t addr) { BR(!sr.carry) }
void CPU::bcs(uint16_t addr) { BR(sr.carry) }
void CPU::beq(uint16_t addr) { BR(sr.zero) }

void CPU::bit(uint16_t addr)
{
	uint8_t n = bus_read(addr);

	sr.neg = GET_BIT(n, 7);
	sr.ov = GET_BIT(n, 6);

	sr.zero = !(n & a);
}

void CPU::bmi(uint16_t addr) { BR(sr.neg) }
void CPU::bne(uint16_t addr) { BR(!sr.zero) }
void CPU::bpl(uint16_t addr) { BR(!sr.neg) }

void CPU::brk(uint16_t addr)
{
	halted = true;
	/*push_word(pc);
	push(sr | 0b110000);
	SET_BIT(sr, true, INT);

	JMP_BUS(IRQL)*/
}

void CPU::bvc(uint16_t addr) { BR(!sr.ov) }
void CPU::bvs(uint16_t addr) { BR(sr.ov) }

void CPU::clc(uint16_t addr) { sr.carry = false; }
void CPU::cld(uint16_t addr) { sr.dec = false; }
void CPU::cli(uint16_t addr) { sr.intr = false; }
void CPU::clv(uint16_t addr) { sr.ov = false; }

void CPU::cmp(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)a + (n ^ 0xff) + 1;

	set_flags(out, true, true, true);
}

void CPU::cpx(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)x + (n ^ 0xff) + 1;

	set_flags(out, true, true, true);
}

void CPU::cpy(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)y + (n ^ 0xff) + 1;

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
	sr.carry = n & 1;
	sr.neg = false;
	uint16_t out = n >> 1;

	set_flags(out, true, true, false);

	bus_write(addr, LSB(out));
}

void CPU::lsr_acc(uint16_t n)
{
	sr.carry = a & 1;
	sr.neg = false;

	set_flags(a >>= 1, false, true, false);
}

void CPU::nop(uint16_t addr) {}

void CPU::ora(uint16_t addr) { set_flags(a |= bus_read(addr), true, true, false); }

void CPU::pha(uint16_t addr) { push(a); }
void CPU::php(uint16_t addr) { push(sr.get() | 0b110000); }
void CPU::pla(uint16_t addr) { set_flags(a = pop(), true, true, false); }
void CPU::plp(uint16_t addr)
{
	// sr &= 0b00110000;
	bool brk = sr.brk;

	sr.set(pop() & 0b11001111);

	sr.brk = brk;
}

void CPU::rol(uint16_t addr)
{
	uint16_t out = ((uint16_t)bus_read(addr) << 1) | sr.carry;

	set_flags(out, true, true, true);

	bus_write(addr, LSB(out));
}

void CPU::rol_acc(uint16_t n)
{
	uint16_t out = ((uint16_t)a << 1) | sr.carry;
	
	a = out & 0xff;

	set_flags(out, true, true, true);
}

void CPU::ror(uint16_t addr)
{
	uint16_t n = bus_read(addr);
	bool set_carry = (n & 1) != 0;
	uint16_t out = n >> 1;
	SET_BIT(out, sr.carry, 7);
	sr.carry = set_carry;

	set_flags(out, true, true, false);

	bus_write(addr, LSB(out));
}

void CPU::ror_acc(uint16_t n)
{
	bool set_carry = (a & 1) != 0;
	a >>= 1;
	SET_BIT(a, sr.carry, 7);
	sr.carry = set_carry;

	set_flags(a, true, true, false);
}

void CPU::rti(uint16_t addr)
{
	sr.set(pop() | 0b100000);
	pc = pop_word();
}

void CPU::rts(uint16_t addr) { pc = pop_word() + 1; }

void CPU::sbc(uint16_t addr)
{
	/*if (GET_BIT(sr, DEC))
	{
		puts("Unimplemented decimal mode sub");
		exit(1);
	}*/

	uint16_t n = bus_read(addr);
	uint16_t out = (uint16_t)a + (n ^ 0xff);
	out += (uint8_t)sr.carry;

	sr.ov = ((a ^ out) & 0x80) & ((a ^ n) & 0x80);
	set_flags(out, true, true, true);

	a = LSB(out);
}

void CPU::sec(uint16_t addr) { sr.carry = true; }
void CPU::sed(uint16_t addr) { sr.dec = true; }
void CPU::sei(uint16_t addr) { sr.intr = true; }

void CPU::sta(uint16_t addr) { bus_write(addr, a); }
void CPU::stx(uint16_t addr) { bus_write(addr, x); }
void CPU::sty(uint16_t addr) { bus_write(addr, y); }

TR(tax, a, x)
TR(tay, a, y)
TR(tsx, sp, x)
TR(txa, x, a)
void CPU::txs(uint16_t addr) { sp = x; }
TR(tya, y, a)

// --- ILLEGAL OPCODES --- //

void CPU::alr(uint16_t addr) { AND(addr); lsr_acc(addr); }
void CPU::anc(uint16_t addr) { set_flags(a &= bus_read(addr), true, true, true); }
void CPU::arr(uint16_t addr) {}
void CPU::dcp(uint16_t addr) { dec(addr); cmp(addr); }
void CPU::isb(uint16_t addr) { inc(addr); sbc(addr); }
void CPU::las(uint16_t addr) {}

void CPU::lax(uint16_t addr)
{
	set_flags(a = x = bus_read(addr), true, true, false);
}

void CPU::lxa(uint16_t addr) {}
void CPU::rla(uint16_t addr) { rol(addr); AND(addr); }
void CPU::rra(uint16_t addr) { ror(addr); adc(addr); }
void CPU::sax(uint16_t addr) { bus_write(addr, a & x); }
void CPU::sbx(uint16_t addr) {}
void CPU::sha(uint16_t addr) {}
void CPU::shx(uint16_t addr) {}
void CPU::shy(uint16_t addr) {}
void CPU::slo(uint16_t addr) { asl(addr); ora(addr); }
void CPU::sre(uint16_t addr) { lsr(addr); eor(addr); }
void CPU::tas(uint16_t addr) {}
