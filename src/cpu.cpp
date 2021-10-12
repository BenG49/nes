#include <cpu.hpp>

#include <iostream>

#ifdef NES_DEBUG
#include <iomanip>
#endif

#define OP(opcode, func, addr, instrs) \
	vec[opcode] = Instr(func, addr, instrs, #func);

CPU::CPU(bus_read_t bus_read, bus_write_t bus_write)
	: a(), x(), y(), sp(), pc(), cycles(), bus_read(bus_read), bus_write(bus_write)
{
	// opcodes
	OP(0x69, &CPU::adc, IMM, 2)
	OP(0x69, &CPU::adc, IMM, 2)
	OP(0x65, &CPU::adc, ZPG, 3)
	OP(0x75, &CPU::adc, ZPX, 4)
	OP(0x6D, &CPU::adc, ABS, 4)
	OP(0x7D, &CPU::adc, ABX, 4)
	OP(0x79, &CPU::adc, ABY, 4)
	OP(0x61, &CPU::adc, INX, 6)
	OP(0x71, &CPU::adc, INY, 5)

	OP(0x29, &CPU::AND, IMM, 2)
	OP(0x25, &CPU::AND, ZPG, 3)
	OP(0x35, &CPU::AND, ZPX, 4)
	OP(0x2D, &CPU::AND, ABS, 4)
	OP(0x3D, &CPU::AND, ABX, 4)
	OP(0x39, &CPU::AND, ABY, 4)
	OP(0x21, &CPU::AND, INX, 6)
	OP(0x31, &CPU::AND, INY, 5)

	OP(0x0A, &CPU::asl_acc, ACC, 2)
	OP(0x06, &CPU::asl, ZPG, 5)
	OP(0x16, &CPU::asl, ZPX, 6)
	OP(0x0E, &CPU::asl, ABS, 6)
	OP(0x1E, &CPU::asl, ABX, 7)

	OP(0x90, &CPU::bcc, REL, 2)
	OP(0xB0, &CPU::bcs, REL, 2)
	OP(0xF0, &CPU::beq, REL, 2)

	OP(0x24, &CPU::bit, ZPG, 3)
	OP(0x2C, &CPU::bit, ABS, 3)

	OP(0xD0, &CPU::bne, REL, 2)
	OP(0x10, &CPU::bpl, REL, 2)

	OP(0x00, &CPU::brk, IMPL, 7)

	OP(0x50, &CPU::bvc, REL, 2)
	OP(0x70, &CPU::bvs, REL, 2)

	OP(0x18, &CPU::clc, IMPL, 2)
	OP(0xD8, &CPU::cld, IMPL, 2)
	OP(0x58, &CPU::cli, IMPL, 2)
	OP(0xB8, &CPU::clv, IMPL, 2)

	OP(0xC9, &CPU::cmp, IMM, 2)
	OP(0xC5, &CPU::cmp, ZPG, 3)
	OP(0xD5, &CPU::cmp, ZPX, 4)
	OP(0xCD, &CPU::cmp, ABS, 5)
	OP(0xDD, &CPU::cmp, ABX, 4)
	OP(0xD9, &CPU::cmp, ABY, 4)
	OP(0xC1, &CPU::cmp, INX, 6)
	OP(0xD1, &CPU::cmp, INY, 5)

	OP(0xE0, &CPU::cpx, IMM, 2)
	OP(0xE4, &CPU::cpx, ZPG, 3)
	OP(0xEC, &CPU::cpx, ABS, 4)

	OP(0xC0, &CPU::cpy, IMM, 2)
	OP(0xC4, &CPU::cpy, ZPG, 3)
	OP(0xCC, &CPU::cpy, ABS, 4)

	OP(0xC6, &CPU::dec, ZPG, 5)
	OP(0xD6, &CPU::dec, ZPX, 6)
	OP(0xCE, &CPU::dec, ABS, 6)
	OP(0xDE, &CPU::dec, ABX, 7)

	OP(0xCA, &CPU::dex, IMPL, 2)
	OP(0x88, &CPU::dey, IMPL, 2)

	OP(0x49, &CPU::eor, IMM, 2)
	OP(0x45, &CPU::eor, ZPG, 3)
	OP(0x55, &CPU::eor, ZPX, 4)
	OP(0x4D, &CPU::eor, ABS, 4)
	OP(0x5D, &CPU::eor, ABX, 4)
	OP(0x59, &CPU::eor, ABY, 4)
	OP(0x41, &CPU::eor, INX, 6)
	OP(0x51, &CPU::eor, INY, 5)

	OP(0xE6, &CPU::inc, ZPG, 5)
	OP(0xF6, &CPU::inc, ZPX, 6)
	OP(0xEE, &CPU::inc, ABS, 6)
	OP(0xFE, &CPU::inc, ABX, 7)

	OP(0x4C, &CPU::jmp, ABS, 3)
	OP(0x6C, &CPU::jmp, IND, 6)

	OP(0x20, &CPU::jsr, ABS, 6)

	OP(0xA9, &CPU::lda, IMM, 2)
	OP(0xA5, &CPU::lda, ZPG, 3)
	OP(0xB5, &CPU::lda, ZPX, 4)
	OP(0xAD, &CPU::lda, ABS, 4)
	OP(0xBD, &CPU::lda, ABX, 4)
	OP(0xB9, &CPU::lda, ABY, 4)
	OP(0xA1, &CPU::lda, INX, 6)
	OP(0xB1, &CPU::lda, INY, 5)

	OP(0xA2, &CPU::ldx, IMM, 2)
	OP(0xA6, &CPU::ldx, ZPG, 3)
	OP(0xB6, &CPU::ldx, ZPY, 4)
	OP(0xAE, &CPU::ldx, ABS, 4)
	OP(0xBE, &CPU::ldx, ABY, 4)

	OP(0xA0, &CPU::ldy, IMM, 2)
	OP(0xA4, &CPU::ldy, ZPG, 3)
	OP(0xB4, &CPU::ldy, ZPY, 4)
	OP(0xAC, &CPU::ldy, ABS, 4)
	OP(0xBC, &CPU::ldy, ABY, 4)

	OP(0x4A, &CPU::lsr_acc, ACC, 2)
	OP(0x46, &CPU::lsr, ZPG, 5)
	OP(0x56, &CPU::lsr, ZPY, 6)
	OP(0x43, &CPU::lsr, ABS, 6)
	OP(0x53, &CPU::lsr, ABY, 7)

	OP(0x09, &CPU::ora, IMM, 2)
	OP(0x05, &CPU::ora, ZPG, 3)
	OP(0x15, &CPU::ora, ZPX, 4)
	OP(0x0D, &CPU::ora, ABS, 4)
	OP(0x1D, &CPU::ora, ABX, 4)
	OP(0x19, &CPU::ora, ABY, 4)
	OP(0x01, &CPU::ora, INX, 6)
	OP(0x11, &CPU::ora, INY, 5)

	OP(0x48, &CPU::pha, IMPL, 3)
	OP(0x08, &CPU::php, IMPL, 3)
	OP(0x68, &CPU::php, IMPL, 4)
	OP(0x28, &CPU::plp, IMPL, 4)

	OP(0x2A, &CPU::rol_acc, ACC, 2)
	OP(0x26, &CPU::rol, ZPG, 5)
	OP(0x36, &CPU::rol, ZPY, 6)
	OP(0x23, &CPU::rol, ABS, 6)
	OP(0x33, &CPU::rol, ABY, 7)

	OP(0x6A, &CPU::ror_acc, ACC, 2)
	OP(0x66, &CPU::ror, ZPG, 5)
	OP(0x76, &CPU::ror, ZPY, 6)
	OP(0x63, &CPU::ror, ABS, 6)
	OP(0x73, &CPU::ror, ABY, 7)

	OP(0x40, &CPU::rti, IMPL, 6)
	OP(0x60, &CPU::rts, IMPL, 6)

	OP(0xE9, &CPU::sbc, IMM, 2)
	OP(0xE9, &CPU::sbc, IMM, 2)
	OP(0xE5, &CPU::sbc, ZPG, 3)
	OP(0xF5, &CPU::sbc, ZPX, 4)
	OP(0xED, &CPU::sbc, ABS, 4)
	OP(0xFD, &CPU::sbc, ABX, 4)
	OP(0xF9, &CPU::sbc, ABY, 4)
	OP(0xE1, &CPU::sbc, INX, 6)
	OP(0xF1, &CPU::sbc, INY, 5)

	OP(0x38, &CPU::sec, IMPL, 2)
	OP(0xF8, &CPU::sed, IMPL, 2)
	OP(0x78, &CPU::sei, IMPL, 2)

	OP(0x85, &CPU::sta, ZPG, 3)
	OP(0x95, &CPU::sta, ZPX, 4)
	OP(0x8D, &CPU::sta, ABS, 4)
	OP(0x9D, &CPU::sta, ABX, 5)
	OP(0x99, &CPU::sta, ABY, 5)
	OP(0x81, &CPU::sta, INX, 6)
	OP(0x91, &CPU::sta, INY, 6)

	OP(0x86, &CPU::stx, ZPG, 3)
	OP(0x96, &CPU::stx, ZPY, 4)
	OP(0x8E, &CPU::stx, ABS, 4)

	OP(0x84, &CPU::sty, ZPG, 3)
	OP(0x94, &CPU::sty, ZPX, 4)
	OP(0x8C, &CPU::sty, ABS, 4)

	OP(0xAA, &CPU::tax, IMPL, 2)
	OP(0xA8, &CPU::tay, IMPL, 2)
	OP(0xBA, &CPU::tsx, IMPL, 2)
	OP(0x8A, &CPU::txa, IMPL, 2)
	OP(0x9A, &CPU::txs, IMPL, 2)
	OP(0x98, &CPU::tya, IMPL, 2)

	reset();
}

void CPU::nmi()
{

}

void CPU::irq()
{

}

void CPU::reset()
{
	cycles = sp = 0;

	push(0);
	push(0);
	push(0);

	JMP_BUS(RSTL);
}

void CPU::exec(int ticks, bool countInstr)
{
	uint8_t op;
	Instr instr;

	#ifdef NES_DEBUG
	uint16_t pc_start;
	#endif

	while (ticks > 0)
	{
		op = bus_read(pc++);

		if (op == 0xFF)
		{
			puts("Program halted");
			exit(0);
		}

		instr = vec[op];

		#ifdef NES_DEBUG

		pc_start = pc - 1;

		printf("%04X  %02X ", pc_start, op);

		#endif

		uint16_t addr;
		switch (instr.a) {
			case IMPL:
			case ACC: break;
			case IMM: addr = pc++; break;
			case ZPG: addr = bus_read(pc++); break;
			case ZPX: addr = bus_read(pc++ + x); break;
			case ZPY: addr = bus_read(pc++ + y); break;
			case ABS:
				addr = bus_read(pc++);
				addr |= (bus_read(pc++) << 8);
				break;
			case ABX:
				addr = bus_read(pc++);
				addr |= (bus_read(pc++) << 8);
				addr += x;
				break;
			case ABY:
				addr = bus_read(pc++);
				addr |= (bus_read(pc++) << 8);
				addr += y;
				break;
			case REL: {
				uint16_t offset = bus_read(pc++);
				if (offset < 128)
					addr = pc + offset;
				else
					addr = pc - (256 - offset);
				break;
			}
			case IND:
				addr = bus_read(pc++);
				addr |= (bus_read(pc++) << 8);
				addr = bus_read(addr);
				break;
			case INX:
				addr = bus_read(pc++);
				addr |= (bus_read(pc++) << 8);
				addr = bus_read(addr + x);
				break;
			case INY:
				addr = bus_read(pc++);
				addr |= (bus_read(pc++) << 8);
				addr = bus_read(addr) + y;
				break;
		}

		(this->*instr.f)(addr);

		#ifdef NES_DEBUG

		for (int i = pc_start + 1; i < pc; ++i)
			printf("%02X ", bus_read(i));
		
		std::cout << ' ' << instr.s;

		printf("  A:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%d\n", a, x, y, sr, sp, cycles);

		#endif

		cycles += instr.cycles;

		if (countInstr) --ticks;
		else ticks -= instr.cycles;
	}
}
