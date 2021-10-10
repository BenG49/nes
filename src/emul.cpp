#include <emul.hpp>

#define IMM(opcode, func) \
	case opcode:          \
		func(op[1]);      \
		++cpu->ip;        \
		break;

#define ZPG(opcode, func)      \
	case opcode:               \
		func(cpu->mem[op[1]]); \
		++cpu->ip;             \
		break;                 \

#define ZPGX(opcode, func)              \
	case opcode:                        \
		func(cpu->mem[op[1] + cpu->x]); \
		++cpu->ip;                      \
		break;

#define ZPGY(opcode, func)              \
	case opcode:                        \
		func(cpu->mem[op[1] + cpu->y]); \
		++cpu->ip;                      \
		break;

#define ABSS(opcode, func)            \
	case opcode:                      \
		func(cpu->mem[addr(op + 1)]); \
		cpu->ip += 2;                 \
		break;

#define ABSX(opcode, func)                     \
	case opcode:                               \
		func(cpu->mem[addr(op + 1) + cpu->x]); \
		cpu->ip += 2;                          \
		break;

#define ABSY(opcode, func)                     \
	case opcode:                               \
		func(cpu->mem[addr(op + 1) + cpu->y]); \
		cpu->ip += 2;                          \
		break;

#define INDX(opcode, func)                          \
	case opcode:                                    \
		func(cpu->mem[get(addr(op + 1) + cpu->x)]); \
		cpu->ip += 2;                               \
		break;

#define INDY(opcode, func)                        \
	case opcode:                                  \
		func(cpu->mem[cpu->mem[op[1]] + cpu->y]); \
		++cpu->ip;                                \
		break;

#define MAIN_ADDR_MODES(imm, zpg, zpgx, abss, absx, absy, indx, indy, func) \
	IMM(imm, func)                                                          \
	ZPG(zpg, func)                                                          \
	ZPGX(zpgx, func)                                                        \
	ABSS(abss, func)                                                        \
	ABSX(absx, func)                                                        \
	ABSY(absy, func)                                                        \
	INDX(indx, func)                                                        \
	INDY(indy, func)

#define BR(opcode, flag)         \
	case opcode:                 \
		++cpu->ip;               \
		if (flag) reljmp(op[1]); \
		break;

#define TR(opcode, to, from) \
	case opcode: cpu->set_flags(to = from, Flags(true, true, false)); break;

#define SET_IMM(opcode, func)

// ---------- //

addr_t Emul::addr(uint8_t *mem) const
{
	return (mem[0] << 8) + mem[1];
}

uint16_t Emul::get(addr_t addr) const
{
	return (cpu->mem[addr] << 8) + cpu->mem[addr + 1];
}

void Emul::reljmp(uint8_t jmp)
{
	if (jmp < 128)
		cpu->ip += jmp;
	else
		cpu->ip -= (256 - jmp);
}

// ---------- //

void Emul::emulate()
{
	uint8_t *op = &cpu->mem[cpu->ip++];

	printf("OP %#04x\n", *op);

	switch (*op) {
		// NOP
		case 0xea: break;

		// ASL a
		case 0x0a:
			cpu->a = asl(cpu->a);
			++cpu->ip;
			break;

		// ASL zeropage
		case 0x06:
			cpu->mem[op[1]] = asl(cpu->mem[op[1]]);
			++cpu->ip;
			break;

		// ASL zeropage, x
		case 0x16:
			cpu->mem[op[1] + cpu->x] = asl(cpu->mem[op[1] + cpu->x]);
			++cpu->ip;
			break;

		// ASL absolute
		case 0x0e:
			cpu->mem[addr(op + 1)] = asl(cpu->mem[addr(op + 1)]);
			cpu->ip += 2;
			break;

		// ASL absolute, x
		case 0x1e:
			cpu->mem[addr(op + 1) + cpu->x] = asl(cpu->mem[addr(op + 1) + cpu->x]);
			cpu->ip += 2;
			break;
		
		// BCC
		BR(0x90, !cpu->f.carry);
		
		// BCS
		BR(0xb0, cpu->f.carry);
		
		// BEQ
		BR(0xf0, cpu->f.zero);
		
		// BIT zeropage
		case 0x24:
			bit(cpu->mem[op[1]]);
			break;
		
		// BIT absolute
		case 0x2c:
			bit(cpu->mem[addr(op + 1)]);
			break;
		
		// BMI
		BR(0x30, cpu->f.neg);
		
		// BNE
		BR(0xd0, !cpu->f.zero);
		
		// BPL
		BR(0x10, !cpu->f.neg);

		// FIXME: pretty sure this is wrong
		// BRK
		case 0x00:
			push(MSB(cpu->ip));
			push(LSB(cpu->ip));
			push(cpu->f.reg());
			cpu->f.intr = true;
			break;
		
		// BVC
		BR(0x50, !cpu->f.ov);
		
		// BVS
		BR(0x70, cpu->f.ov);

		// CLC
		case 0x18: cpu->f.carry = false; break;

		// CLD
		case 0xd8: cpu->f.dec = false; break;

		// CLI
		case 0x58: cpu->f.intr = false; break;

		// CLV
		case 0xb8: cpu->f.ov = false; break;

		// CPX
		IMM(0xe0, cpx)
		ZPG(0xe4, cpx)
		ABSS(0xec, cpx)

		// CPY
		IMM(0xc0, cpx)
		ZPG(0xc4, cpx)
		ABSS(0xcc, cpx)

		// DEC
		case 0xc6: cpu->set_flags(--cpu->mem[op[1]], 		  		 Flags(true, true, false)); break;
		case 0xd6: cpu->set_flags(--cpu->mem[op[1] + cpu->x], 		 Flags(true, true, false)); break;
		case 0xce: cpu->set_flags(--cpu->mem[addr(op + 1)], 		 Flags(true, true, false)); break;
		case 0xde: cpu->set_flags(--cpu->mem[addr(op + 1) + cpu->x], Flags(true, true, false)); break;

		// INC
		case 0xe6: cpu->set_flags(++cpu->mem[op[1]], 		  		 Flags(true, true, false)); break;
		case 0xf6: cpu->set_flags(++cpu->mem[op[1] + cpu->x], 		 Flags(true, true, false)); break;
		case 0xee: cpu->set_flags(++cpu->mem[addr(op + 1)], 		 Flags(true, true, false)); break;
		case 0xfe: cpu->set_flags(++cpu->mem[addr(op + 1) + cpu->x], Flags(true, true, false)); break;

		// DEX
		case 0xca: cpu->set_flags(--cpu->x, Flags(true, true, false)); break;

		// INX
		case 0xe8: cpu->set_flags(++cpu->x, Flags(true, true, false)); break;

		// DEY
		case 0x88: cpu->set_flags(--cpu->y, Flags(true, true, false)); break;

		// INY
		case 0xc8: cpu->set_flags(++cpu->y, Flags(true, true, false)); break;

		// JMP absolute
		case 0x4c: cpu->ip = addr(op + 1); break;

		// JMP indirect
		case 0x6c: cpu->ip = get(addr(op + 1)); break;

		// JSR
		case 0x20:
			push(MSB(cpu->ip));
			push(LSB(cpu->ip));
			cpu->ip = addr(op + 1);
			break;

		// LDX
		IMM(0xa2, ldx)
		ZPG(0xa6, ldx)
		ZPGY(0xb6, ldx)
		ABSS(0xae, ldx)
		ABSY(0xbe, ldx)

		// LDY
		IMM(0xa0, ldx)
		ZPG(0x46, ldx)
		ZPGY(0x56, ldx)
		ABSS(0x4e, ldx)
		ABSY(0x5e, ldx)

		// PHA
		case 0x48: push(cpu->a); break;

		// PHP
		case 0x08: push(cpu->f.reg()); break;

		// PLA
		case 0x68: cpu->set_flags(cpu->a = pop(), Flags(true, true, false)); break;

		// PLP
		case 0x28:
			cpu->f.set(pop());
			cpu->set_flags(cpu->f.reg(), Flags(true, true, false));
			break;
		
		// ROL a
		case 0x2a: cpu->a = rol(cpu->a); break;
		
		// ROL zeropage
		case 0x26: cpu->mem[op[1]] = rol(cpu->mem[op[1]]); break;
		
		// ROL zeropage, x
		case 0x36: cpu->mem[op[1] + cpu->x] = rol(cpu->mem[op[1] + cpu->x]); break;
		
		// ROL abs
		case 0x2e: cpu->mem[addr(op + 1)] = rol(cpu->mem[addr(op + 1)]); break;
		
		// ROL abs, x
		case 0x3e: cpu->mem[addr(op + 1) + cpu->x] = rol(cpu->mem[addr(op + 1) + cpu->x]); break;
		
		// ROR a
		case 0x6a: cpu->a = ror(cpu->a); break;
		
		// ROR zeropage
		case 0x66: cpu->mem[op[1]] = ror(cpu->mem[op[1]]); break;
		
		// ROR zeropage, x
		case 0x76: cpu->mem[op[1] + cpu->x] = ror(cpu->mem[op[1] + cpu->x]); break;
		
		// ROR abs
		case 0x6e: cpu->mem[addr(op + 1)] = ror(cpu->mem[addr(op + 1)]); break;
		
		// ROR abs, x
		case 0x7e: cpu->mem[addr(op + 1) + cpu->x] = ror(cpu->mem[addr(op + 1) + cpu->x]); break;

		// RTI
		case 0x40:
			cpu->f.set_int(pop());
			cpu->ip = pop_word();
			break;

		// RTS
		case 0x60:
			cpu->ip = pop_word();
			break;
		
		// SEC
		case 0x38: cpu->f.carry = true; break;

		// SED
		case 0xfb: cpu->f.dec = true; break;

		// SEI
		case 0x78: cpu->f.intr = true; break;

		// STA
		case 0x85: cpu->mem[op[1]] = cpu->a; break;
		case 0x95: cpu->mem[op[1] + cpu->x] = cpu->a; break;
		case 0x8d: cpu->mem[addr(op + 1)] = cpu->a; break;
		case 0x9d: cpu->mem[addr(op + 1) + cpu->x] = cpu->a; break;
		case 0x81: cpu->mem[get(addr(op + 1) + cpu->x)] = cpu->a; break;
		case 0x91: cpu->mem[cpu->mem[op[1]] + cpu->y] = cpu->a; break;

		// STX
		case 0x86: cpu->mem[op[1]] = cpu->x; break;
		case 0x96: cpu->mem[op[1] + cpu->y] = cpu->x; break;
		case 0x8e: cpu->mem[addr(op + 1)] = cpu->x; break;

		// STY
		case 0x84: cpu->mem[op[1]] = cpu->y; break;
		case 0x94: cpu->mem[op[1] + cpu->x] = cpu->y; break;
		case 0x8c: cpu->mem[addr(op + 1)] = cpu->y; break;

		// TAX
		TR(0xaa, cpu->x, cpu->a)

		// TAY
		TR(0xa8, cpu->y, cpu->a)

		// TSX
		TR(0xba, cpu->x, cpu->sp)

		// TXA
		TR(0x8a, cpu->a, cpu->a)

		// TXS
		TR(0x9a, cpu->sp, cpu->x)

		// TYA
		TR(0x98, cpu->a, cpu->y)

		// ADC
		MAIN_ADDR_MODES(0x69, 0x65, 0x75, 0x6d, 0x7d, 0x79, 0x61, 0x71, add)

		// AND
		MAIN_ADDR_MODES(0x29, 0x25, 0x35, 0x2d, 0x3d, 0x39, 0x21, 0x31, and_f)

		// CMP
		MAIN_ADDR_MODES(0xc9, 0xc5, 0xd5, 0xcd, 0xdd, 0xd9, 0xc1, 0xd1, cmp);

		// EOR
		MAIN_ADDR_MODES(0x49, 0x45, 0x55, 0x4d, 0x5d, 0x59, 0x41, 0x51, eor)

		// LDA
		MAIN_ADDR_MODES(0xa9, 0xa5, 0xb5, 0xad, 0xbd, 0xb9, 0xa1, 0xb1, lda)

		// ORA
		MAIN_ADDR_MODES(0x09, 0x05, 0x15, 0x0d, 0x1d, 0x19, 0x01, 0x11, ora)

		// SBC
		MAIN_ADDR_MODES(0xe9, 0xe5, 0xf5, 0xed, 0xfd, 0xf9, 0xe1, 0xf1, sbc)
	
		case 0xff:
			puts("Program halted");
			exit(0);

		default:
			printf("Unimplemented opcode %#04x\n", *op);
			exit(1);
	}

	puts("------------");
	printf("A=%#04x X=%#04x Y=%#04x SP=%#04x IP=%#04x\n", cpu->a, cpu->x, cpu->y, cpu->sp, cpu->ip);
	cpu->f.print();
	printf("%#04x\n", cpu->mem[0]);
	puts("------------");
}
