#include <cpu.hpp>

#include <iostream>

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
	OP(0x6d, &CPU::adc, ABS, 4)
	OP(0x7d, &CPU::adc, ABX, 4)
	OP(0x79, &CPU::adc, ABY, 4)
	OP(0x61, &CPU::adc, INX, 6)
	OP(0x71, &CPU::adc, INY, 5)

	OP(0x29, &CPU::_and, IMM, 2)
	OP(0x25, &CPU::_and, ZPG, 3)
	OP(0x35, &CPU::_and, ZPX, 4)
	OP(0x2d, &CPU::_and, ABS, 4)
	OP(0x3d, &CPU::_and, ABX, 4)
	OP(0x39, &CPU::_and, ABY, 4)
	OP(0x21, &CPU::_and, INX, 6)
	OP(0x31, &CPU::_and, INY, 5)

	OP(0x0a, &CPU::asl, ACC, 2)
	OP(0x06, &CPU::asl, ZPG, 5)
	OP(0x16, &CPU::asl, ZPX, 6)
	OP(0x0e, &CPU::asl, ABS, 6)
	OP(0x1e, &CPU::asl, ABX, 7)

	OP(0x90, &CPU::bcc, REL, 2)
	OP(0xb0, &CPU::bcs, REL, 2)
	OP(0xf0, &CPU::beq, REL, 2)

	OP(0x24, &CPU::bit, ZPG, 3)
	OP(0x2c, &CPU::bit, ABS, 3)

	OP(0xd0, &CPU::bne, REL, 2)
	OP(0x10, &CPU::bpl, REL, 2)

	OP(0x00, &CPU::brk, IMPL, 7)

	OP(0x50, &CPU::bvc, REL, 2)
	OP(0x70, &CPU::bvs, REL, 2)

	OP(0x18, &CPU::clc, IMPL, 2)
	OP(0xd8, &CPU::cld, IMPL, 2)
	OP(0x58, &CPU::cli, IMPL, 2)
	OP(0xb8, &CPU::clv, IMPL, 2)

	OP(0xc9, &CPU::cmp, IMM, 2)
	OP(0xc5, &CPU::cmp, ZPG, 3)
	OP(0xd5, &CPU::cmp, ZPX, 4)
	OP(0xcd, &CPU::cmp, ABS, 5)
	OP(0xdd, &CPU::cmp, ABX, 4)
	OP(0xd9, &CPU::cmp, ABY, 4)
	OP(0xc1, &CPU::cmp, INX, 6)
	OP(0xd1, &CPU::cmp, INY, 5)

	OP(0xe0, &CPU::cpx, IMM, 2)
	OP(0xe4, &CPU::cpx, ZPG, 3)
	OP(0xec, &CPU::cpx, ABS, 4)

	OP(0xc0, &CPU::cpy, IMM, 2)
	OP(0xc4, &CPU::cpy, ZPG, 3)
	OP(0xcc, &CPU::cpy, ABS, 4)

	OP(0xc6, &CPU::dec, ZPG, 5)
	OP(0xd6, &CPU::dec, ZPX, 6)
	OP(0xce, &CPU::dec, ABS, 6)
	OP(0xde, &CPU::dec, ABX, 7)

	OP(0xca, &CPU::dex, IMPL, 2)
	OP(0x88, &CPU::dey, IMPL, 2)

	OP(0x49, &CPU::eor, IMM, 2)
	OP(0x45, &CPU::eor, ZPG, 3)
	OP(0x55, &CPU::eor, ZPX, 4)
	OP(0x4d, &CPU::eor, ABS, 4)
	OP(0x5d, &CPU::eor, ABX, 4)
	OP(0x59, &CPU::eor, ABY, 4)
	OP(0x41, &CPU::eor, INX, 6)
	OP(0x51, &CPU::eor, INY, 5)

	OP(0xe6, &CPU::inc, ZPG, 5)
	OP(0xf6, &CPU::inc, ZPX, 6)
	OP(0xee, &CPU::inc, ABS, 6)
	OP(0xfe, &CPU::inc, ABX, 7)

	OP(0x4c, &CPU::jmp, ABS, 3)
	OP(0x6c, &CPU::jmp, IND, 6)

	OP(0x20, &CPU::jsr, ABS, 6)

	OP(0xa9, &CPU::lda, IMM, 2)
	OP(0xa5, &CPU::lda, ZPG, 3)
	OP(0xb5, &CPU::lda, ZPX, 4)
	OP(0xad, &CPU::lda, ABS, 4)
	OP(0xbd, &CPU::lda, ABX, 4)
	OP(0xb9, &CPU::lda, ABY, 4)
	OP(0xa1, &CPU::lda, INX, 6)
	OP(0xb1, &CPU::lda, INY, 5)

	OP(0xa2, &CPU::ldx, IMM, 2)
	OP(0xa6, &CPU::ldx, ZPG, 3)
	OP(0xb6, &CPU::ldx, ZPY, 4)
	OP(0xae, &CPU::ldx, ABS, 4)
	OP(0xbe, &CPU::ldx, ABY, 4)

	OP(0xa0, &CPU::ldy, IMM, 2)
	OP(0xa4, &CPU::ldy, ZPG, 3)
	OP(0xb4, &CPU::ldy, ZPY, 4)
	OP(0xac, &CPU::ldy, ABS, 4)
	OP(0xbc, &CPU::ldy, ABY, 4)

	OP(0x4a, &CPU::lsr, IMM, 2)
	OP(0x46, &CPU::lsr, ZPG, 5)
	OP(0x56, &CPU::lsr, ZPY, 6)
	OP(0x43, &CPU::lsr, ABS, 6)
	OP(0x53, &CPU::lsr, ABY, 7)

	OP(0x09, &CPU::ora, IMM, 2)
	OP(0x05, &CPU::ora, ZPG, 3)
	OP(0x15, &CPU::ora, ZPX, 4)
	OP(0x0d, &CPU::ora, ABS, 4)
	OP(0x1d, &CPU::ora, ABX, 4)
	OP(0x19, &CPU::ora, ABY, 4)
	OP(0x01, &CPU::ora, INX, 6)
	OP(0x11, &CPU::ora, INY, 5)

	OP(0x48, &CPU::pha, IMPL, 3)
	OP(0x08, &CPU::php, IMPL, 3)
	OP(0x68, &CPU::php, IMPL, 4)
	OP(0x28, &CPU::plp, IMPL, 4)

	OP(0x2a, &CPU::rol, IMM, 2)
	OP(0x26, &CPU::rol, ZPG, 5)
	OP(0x36, &CPU::rol, ZPY, 6)
	OP(0x23, &CPU::rol, ABS, 6)
	OP(0x33, &CPU::rol, ABY, 7)

	OP(0x6a, &CPU::ror, IMM, 2)
	OP(0x66, &CPU::ror, ZPG, 5)
	OP(0x76, &CPU::ror, ZPY, 6)
	OP(0x63, &CPU::ror, ABS, 6)
	OP(0x73, &CPU::ror, ABY, 7)

	OP(0x40, &CPU::rti, IMPL, 6)
	OP(0x60, &CPU::rts, IMPL, 6)

	OP(0xe9, &CPU::sbc, IMM, 2)
	OP(0xe9, &CPU::sbc, IMM, 2)
	OP(0xe5, &CPU::sbc, ZPG, 3)
	OP(0xf5, &CPU::sbc, ZPX, 4)
	OP(0xed, &CPU::sbc, ABS, 4)
	OP(0xfd, &CPU::sbc, ABX, 4)
	OP(0xf9, &CPU::sbc, ABY, 4)
	OP(0xe1, &CPU::sbc, INX, 6)
	OP(0xf1, &CPU::sbc, INY, 5)

	OP(0x38, &CPU::sec, IMPL, 2)
	OP(0xf8, &CPU::sed, IMPL, 2)
	OP(0x78, &CPU::sei, IMPL, 2)

	OP(0x85, &CPU::sta, ZPG, 3)
	OP(0x95, &CPU::sta, ZPX, 4)
	OP(0x8d, &CPU::sta, ABS, 4)
	OP(0x9d, &CPU::sta, ABX, 5)
	OP(0x99, &CPU::sta, ABY, 5)
	OP(0x81, &CPU::sta, INX, 6)
	OP(0x91, &CPU::sta, INY, 6)

	OP(0x86, &CPU::stx, ZPG, 3)
	OP(0x96, &CPU::stx, ZPY, 4)
	OP(0x8e, &CPU::stx, ABS, 4)

	OP(0x84, &CPU::sty, ZPG, 3)
	OP(0x94, &CPU::sty, ZPX, 4)
	OP(0x8c, &CPU::sty, ABS, 4)

	OP(0xaa, &CPU::tax, IMPL, 2)
	OP(0xa8, &CPU::tay, IMPL, 2)
	OP(0xba, &CPU::tsx, IMPL, 2)
	OP(0x8a, &CPU::txa, IMPL, 2)
	OP(0x9a, &CPU::txs, IMPL, 2)
	OP(0x98, &CPU::tya, IMPL, 2)
}

void CPU::nmi()
{

}

void CPU::irq()
{

}

void CPU::reset()
{
	a = x = y = sr = cycles = 0;
	sp = 0xff;

	JMP_BUS(IRQL);
}

void CPU::exec(int ticks, bool countInstr)
{
	while (ticks > 0)
	{
		uint8_t op = bus_read(pc++);

		if (op == 0xff)
		{
			puts("Program halted");
			exit(0);
		}

		Instr instr = vec[op];

		printf("%#04x: ", pc - 1);
		std::cout << instr.s; //<< '\n';

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

		printf("\tA=0x%02x X=0x%02x Y=0x%02x SP=0x%02x - ", a, x, y, sp);
		printf("N=%d V=%d B=%d D=%d I=%d Z=%d C=%d\n",
			GET_BIT(sr, NEG),
			GET_BIT(sr, OV),
			GET_BIT(sr, BRK),
			GET_BIT(sr, DEC),
			GET_BIT(sr, INT),
			GET_BIT(sr, ZERO),
			GET_BIT(sr, CARRY));
		
		/*printf("\tZPG 0x00-0x04:");
		for (int i = 0; i < 4; ++i) {
			if (!(i % 2))
				printf(" ");
			printf("%02x", bus_read(i));
		}
		printf("\n");*/

		cycles += instr.cycles;

		if (countInstr) --ticks;
		else ticks -= instr.cycles;
	}
}
