#include <interpret.hpp>

#define IMM(imm, func) \
	case imm:          \
		func(op[1]);   \
		++cpu->ip;     \
		break;

#define ZPG(zpg, func)         \
	case zpg:                  \
		func(cpu->mem[op[1]]); \
		++cpu->ip;             \
		break;                 \

#define ZPGX(zpgx, func)                \
	case zpgx:                          \
		func(cpu->mem[op[1] + cpu->x]); \
		++cpu->ip;                      \
		break;

#define ABSS(abss, func)              \
	case abss:                        \
		func(cpu->mem[addr(op + 1)]); \
		cpu->ip += 2;                 \
		break;

#define ABSX(absx, func)                       \
	case absx:                                 \
		func(cpu->mem[addr(op + 1) + cpu->x]); \
		cpu->ip += 2;                          \
		break;

#define ABSY(absy, func)                       \
	case absy:                                 \
		func(cpu->mem[addr(op + 1) + cpu->y]); \
		cpu->ip += 2;                          \
		break;

#define INDX(indx, func)                                 \
	case indx:                                           \
		func(cpu->mem[get(addr(op + 1) + cpu->x)]);      \
		cpu->ip += 2;                                    \
		break;

#define INDY(indy, func)                          \
	case indy:                                    \
		func(cpu->mem[cpu->mem[op[1]] + cpu->y]); \
		++cpu->ip;                                \
		break;

#define ALL_ADDR_MODES(imm, zpg, zpgx, abss, absx, absy, indx, indy, func) \
	IMM(imm, func)                                                         \
	ZPG(zpg, func)                                                         \
	ZPGX(zpgx, func)                                                       \
	ABSS(abss, func)                                                       \
	ABSX(absx, func)                                                       \
	ABSY(absy, func)                                                       \
	INDX(indx, func)                                                       \
	INDY(indy, func)

// ---------- //

addr_t Interpret::addr(uint8_t *mem) const
{
	return (mem[0] << 8) + mem[1];
}

uint16_t Interpret::get(addr_t addr) const
{
	return (cpu->mem[addr] << 8) + cpu->mem[addr + 1];
}

// ---------- //

void Interpret::add_f(cpu_t n)
{
	uint16_t out = (uint16_t)cpu->a + (uint16_t)n + cpu->f.carry;

	cpu->f.ov = GET_BIT((cpu->a ^ out) & (n ^ out), 7);
	cpu->set_flags(out, Flags(true, true, true));

	cpu->a = out;
}

void Interpret::and_f(cpu_t n)
{
	cpu->a = cpu->a & n;

	cpu->set_flags(cpu->a, Flags(true, true, false));
}

uint8_t Interpret::asl_f(cpu_t n)
{
	uint16_t out = ((uint16_t)n) << 1;

	cpu->set_flags(out, Flags(true, true, true));

	return out;
}

void Interpret::cmp_f(cpu_t n)
{
	uint16_t out = (uint16_t)cpu->a - (uint16_t)n;

	cpu->set_flags(out, Flags(true, true, true));
}

void Interpret::eor_f(cpu_t n)
{
	cpu->a = cpu->a ^ n;

	cpu->set_flags(cpu->a, Flags(true, true, false));
}

void Interpret::lda_f(cpu_t n)
{
	cpu->a = n;

	cpu->set_flags(cpu->a, Flags(true, true, false));
}

void Interpret::ora_f(cpu_t n)
{
	cpu->a = cpu->a | n;

	cpu->set_flags(cpu->a, Flags(true, true, false));
}

void Interpret::sbc_f(cpu_t n)
{
	uint16_t out = (uint16_t)cpu->a - (uint16_t)n - cpu->f.carry;

	cpu->f.ov = GET_BIT((cpu->a ^ out) & ((255 - n) ^ out), 7);
	cpu->set_flags(out, Flags(true, true, true));

	cpu->a = out;
}

// ---------- //

void Interpret::interpret()
{
	uint8_t *op = &cpu->mem[cpu->ip++];

	printf("OP %#04x\n", *op);

	switch (*op) {
		// NOP
		case 0xea: break;

		// ASL a
		case 0x0a:
			cpu->a = asl_f(cpu->a);
			++cpu->ip;
			break;

		// ASL zeropage
		case 0x06:
			cpu->mem[op[1]] = asl_f(cpu->mem[op[1]]);
			++cpu->ip;
			break;

		// ASL zeropage, x
		case 0x16:
			cpu->mem[op[1] + cpu->x] = asl_f(cpu->mem[op[1] + cpu->x]);
			++cpu->ip;
			break;

		// ASL absolute
		case 0x0e:
			cpu->mem[addr(op + 1)] = asl_f(cpu->mem[addr(op + 1)]);
			cpu->ip += 2;
			break;

		// ASL absolute, x
		case 0x1e:
			cpu->mem[addr(op + 1) + cpu->x] = asl_f(cpu->mem[addr(op + 1) + cpu->x]);
			cpu->ip += 2;
			break;

		// ADC all addr modes
		ALL_ADDR_MODES(0x69, 0x65, 0x75, 0x6d, 0x7d, 0x79, 0x61, 0x71, add_f)

		// AND all addr modes
		ALL_ADDR_MODES(0x29, 0x25, 0x35, 0x2d, 0x3d, 0x39, 0x21, 0x31, and_f)

		// CMP all addr modes
		// ALL_ADDR_MODES(0x29, 0x25, 0x35, 0x2d, 0x3d, 0x39, 0x21, 0x31, cmp_f)

		// EOR all addr modes
		ALL_ADDR_MODES(0x49, 0x45, 0x55, 0x4d, 0x5d, 0x59, 0x41, 0x51, eor_f)

		// LDA all addr modes
		ALL_ADDR_MODES(0xa9, 0xa5, 0xb5, 0xad, 0xbd, 0xb9, 0xa1, 0xb1, lda_f)

		// ORA all addr modes
		ALL_ADDR_MODES(0x09, 0x05, 0x15, 0x0d, 0x1d, 0x19, 0x01, 0x11, ora_f)

		// SBC all addr modes
		ALL_ADDR_MODES(0xe9, 0xe5, 0xf5, 0xed, 0xfd, 0xf9, 0xe1, 0xf1, sbc_f)

		default:
			printf("Unimplemented opcode %#04x\n", *op);
			exit(1);
	}

	puts("------------");
	printf("A %#04x X %#04x Y %#04x\n", cpu->a, cpu->x, cpu->y);
	cpu->f.print();
	puts("------------");
}
