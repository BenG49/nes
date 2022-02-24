#include <cpu.hpp>

#include <iostream>

#include <algorithm>
#include <iomanip>
#include <bitset>

#define OP(opcode, func, addr, instrs) \
	vec[opcode] = Instr(func, addr, instrs, #func);

CPU::CPU(bus_read_t bus_read, bus_write_t bus_write)
	: a(), x(), y(), sp(), pc(), cycles(), bus_read(bus_read), bus_write(bus_write), halted(false)
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
	OP(0x30, &CPU::bmi, REL, 2)

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
	OP(0xB4, &CPU::ldy, ZPX, 4)
	OP(0xAC, &CPU::ldy, ABS, 4)
	OP(0xBC, &CPU::ldy, ABX, 4)

	OP(0x4A, &CPU::lsr_acc, ACC, 2)
	OP(0x46, &CPU::lsr, ZPG, 5)
	OP(0x56, &CPU::lsr, ZPX, 6)
	OP(0x4E, &CPU::lsr, ABS, 6)
	OP(0x5E, &CPU::lsr, ABX, 7)

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
	OP(0x68, &CPU::pla, IMPL, 4)
	OP(0x28, &CPU::plp, IMPL, 4)

	OP(0x2A, &CPU::rol_acc, ACC, 2)
	OP(0x26, &CPU::rol, ZPG, 5)
	OP(0x36, &CPU::rol, ZPX, 6)
	OP(0x2E, &CPU::rol, ABS, 6)
	OP(0x3E, &CPU::rol, ABX, 7)

	OP(0x6A, &CPU::ror_acc, ACC, 2)
	OP(0x66, &CPU::ror, ZPG, 5)
	OP(0x76, &CPU::ror, ZPX, 6)
	OP(0x6E, &CPU::ror, ABS, 6)
	OP(0x7E, &CPU::ror, ABX, 7)

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
	OP(0xE8, &CPU::inx, IMPL, 2);

	OP(0x84, &CPU::sty, ZPG, 3)
	OP(0x94, &CPU::sty, ZPX, 4)
	OP(0x8C, &CPU::sty, ABS, 4)
	OP(0xC8, &CPU::iny, IMPL, 2);

	OP(0xAA, &CPU::tax, IMPL, 2)
	OP(0xA8, &CPU::tay, IMPL, 2)
	OP(0xBA, &CPU::tsx, IMPL, 2)
	OP(0x8A, &CPU::txa, IMPL, 2)
	OP(0x9A, &CPU::txs, IMPL, 2)
	OP(0x98, &CPU::tya, IMPL, 2)
}

// irq but cannot be disabled
void CPU::nmi()
{
	SET_BIT(sr, false, BRK);

	push_word(pc);

	push(sr | 0b100000);

	SET_BIT(sr, true, INT);

	JMP_BUS(IRQL);
}

void CPU::irq()
{
	// interrupt not disabled
	if (!GET_BIT(sr, INT))
	{
		SET_BIT(sr, false, BRK);

		push_word(pc);

		push(sr | 0b100000);

		SET_BIT(sr, true, INT);

		JMP_BUS(IRQL);
	}
}

void CPU::reset()
{
	a = y = x = 0;
	sr = 0b00100100;

	JMP_BUS(RSTL)

	sp = 0xFD;
}
void CPU::step()
{
	uint8_t op = bus_read(pc++);
	Instr instr = vec[op];

	uint16_t addr;
	switch (instr.addr_mode) {
		case IMPL:
		case ACC: break;
		case IMM: addr = pc++; break;
		case ZPG: addr = bus_read(pc++); break;
		case ZPX: addr = (bus_read(pc++) + x) & 0xff; break;
		case ZPY: addr = (bus_read(pc++) + y) & 0xff; break;
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
		case IND: {
			uint16_t base = bus_read(pc++);
			base |= bus_read(pc++) << 8;

			addr = bus_read(base);
			addr |= bus_read((base & 0xff00) | ((base + 1) & 0xff)) << 8;

			break;
		}
		case INX: {
			uint8_t base = bus_read(pc++) + x;

			addr = bus_read(base) | (bus_read((base + 1) & 0xff) << 8);
			break;
		}
		case INY: {
			uint8_t base = bus_read(pc++);

			addr = bus_read(base) | (bus_read((base + 1) & 0xff) << 8);
			addr += y;
			break;
		}
	}

	(this->*instr.func)(addr);

	cycles += instr.cycles;
}

void CPU::exec_with_callback(std::function<void(CPU *)> callback)
{
	while (!halted)
	{
		callback(this);
		step();
	}
}

void CPU::exec()
{
	while (!halted) step();
}

void CPU::set_read(bus_read_t br) { bus_read = br; }
void CPU::set_write(bus_write_t bw) { bus_write = bw; }

std::string CPU::disas(uint8_t instr, uint16_t args)
{
	CPU::Instr i = vec[instr];

	std::string out;

	out += i.name;

	if (i.addr_mode == IMPL)
		return out;

	out += " ";

	switch (i.addr_mode)
	{
		case ACC:
			out += "A";
			break;
		case IMM:
			out += "#$" + to_hex(args, 1);
			break;
		case ZPG:
			out += "$" + to_hex(args, 1) + " = " + to_hex(bus_read(args), 1);
			break;
		case ZPX:
			out += "$" + to_hex(args, 1) + ",X @ " + to_hex((args + x) & 0xff, 1) + " = " + to_hex(bus_read((args + x) & 0xff), 1);
			break;
		case ZPY:
			out += "$" + to_hex(args, 1) + ",Y @ " + to_hex((args + y) & 0xff, 1) + " = " + to_hex(bus_read((args + y) & 0xff), 1);
			break;
		case IND: {
			uint16_t ind = bus_read(args) | (bus_read((args & 0xff00) | ((args + 1) & 0xff)) << 8);

			out += "($" + to_hex(args, 2) + ") = " + to_hex(ind, 2);

			// all indirect jumps don't print value at jump addr
			if (i.name[0] != 'J')
				out += " = " + to_hex(bus_read(ind), 1);
			break;
		}
		case INX: {
			uint16_t ind = bus_read((args + x) & 0xff) | (bus_read((args + x + 1) & 0xff) << 8);

			out += "($" + to_hex(args, 1) + ",X) @ " + to_hex((args + x) & 0xff, 1) + " = " + to_hex(ind, 2) + " = " + to_hex(bus_read(ind), 1);
			break;
		}
		case INY: {
			uint16_t ind = bus_read(args) | (bus_read((args + 1) & 0xff) << 8);

			out += "($" + to_hex(args, 1) + "),Y = " + to_hex(ind, 2) + " @ " + to_hex(ind + y, 2) + " = " + to_hex(bus_read(ind + y), 1);
			break;
		}
		case ABS:
			out += "$" + to_hex(args, 2);

			// all absolute jumps don't print value at jump addr
			if (i.name[0] != 'J')
				out += " = " + to_hex(bus_read(args), 1);
			
			break;
		case ABX:
			out += "$" + to_hex(args, 2) + ",X @ " + to_hex(args + x, 2) + " = " + to_hex(bus_read(args + x), 1);
			break;
		case ABY:
			out += "$" + to_hex(args, 2) + ",Y @ " + to_hex(args + y, 2) + " = " + to_hex(bus_read(args + y), 1);
			break;
		case REL:
			out += "$" + to_hex(args + pc + 2, 2);
			break;
	}

	return out;
}

int CPU::instr_bytes(AddrMode mode)
{
	switch (mode)
	{
		case ACC:
		case IMPL:
			return 1;
		case ABS:
		case ABX:
		case ABY:
		case IND:
			return 3;
		default:
			return 2;
	}
}
