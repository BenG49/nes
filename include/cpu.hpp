#pragma once

#include <algorithm>
#include <functional>
#include <cstdint>
#include <string>

#include <util.hpp>

const uint8_t NEG   = 7;
const uint8_t OV    = 6;
const uint8_t CONST = 5;
const uint8_t BRK   = 4;
const uint8_t DEC   = 3;
const uint8_t INT   = 2;
const uint8_t ZERO  = 1;
const uint8_t CARRY = 0;

class CPU;

typedef void (CPU::*op_t)(uint16_t);

enum AddrMode {
	ACC, IMM, ZPG, ZPX, ZPY, ABS, ABX, ABY, IMPL, REL, IND, INX, INY
};

class CPU {
public:
	// NV-BDIZC
	struct Status {
		bool neg, ov, brk, dec, intr, zero, carry;

		uint8_t get() {
			return carry |
			       (zero << 1) |
				   (intr << 2) |
				   (dec << 3) |
				   (brk << 4) |
				   (1 << 5) |
				   (ov << 6) |
				   (neg << 7);
		}

		void set(uint8_t val) {
			neg = GET_BIT(val, 7);
			ov =  GET_BIT(val, 6);
			brk = GET_BIT(val, 4);
			dec = GET_BIT(val, 3);
			intr = GET_BIT(val, 2);
			zero = GET_BIT(val, 1);
			carry = val & 1;
		}

		Status() {}
	};

private:
	// stack pointer = 0x100 + sp, grows downward
	uint8_t a, x, y, sp;
	Status sr;

	uint16_t pc;

	size_t cycles;
	
	struct Instr {
		op_t func;
		AddrMode addr_mode;
	
		uint8_t cycles;

		std::string name;

		bool valid;
	
		Instr() : func(&CPU::nop), addr_mode(IMPL), cycles(2), name("NOP"), valid(true) {}
		Instr(op_t f, AddrMode a, uint8_t cycles, const char *str, bool valid)
			: func(f), addr_mode(a), cycles(cycles), name(str), valid(valid)
		{
			// substring is used because name is passed in as "&CPU::name", couldn't be bothered to fix
			// opcode names are 3 characters long
			name = name.substr(6, 3);
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		}
	};

	Instr vec[256];

	bus_read_t bus_read;
	bus_write_t bus_write;
	
	void set_flags(uint16_t val, bool neg, bool zero, bool carry);

	void push(uint8_t n);
	void push_word(uint16_t n);
	uint8_t pop();
	uint16_t pop_word();

	void adc(uint16_t addr);
	void AND(uint16_t addr);
	void asl(uint16_t addr); void asl_acc(uint16_t n);
	void bcc(uint16_t addr);
	void bcs(uint16_t addr);
	void beq(uint16_t addr);
	void bit(uint16_t addr);
	void bmi(uint16_t addr);
	void bne(uint16_t addr);
	void bpl(uint16_t addr);
	void brk(uint16_t addr);
	void bvc(uint16_t addr);
	void bvs(uint16_t addr);
	void clc(uint16_t addr);
	void cld(uint16_t addr);
	void cli(uint16_t addr);
	void clv(uint16_t addr);
	void cmp(uint16_t addr);
	void cpx(uint16_t addr);
	void cpy(uint16_t addr);
	void dec(uint16_t addr);
	void dex(uint16_t addr);
	void dey(uint16_t addr);
	void eor(uint16_t addr);
	void inc(uint16_t addr);
	void inx(uint16_t addr);
	void iny(uint16_t addr);
	void jmp(uint16_t addr);
	void jsr(uint16_t addr);
	void lda(uint16_t addr);
	void ldx(uint16_t addr);
	void ldy(uint16_t addr);
	void lsr(uint16_t addr); void lsr_acc(uint16_t n);
	void nop(uint16_t addr);
	void ora(uint16_t addr);
	void pha(uint16_t addr);
	void php(uint16_t addr);
	void pla(uint16_t addr);
	void plp(uint16_t addr);
	void rol(uint16_t addr); void rol_acc(uint16_t n);
	void ror(uint16_t addr); void ror_acc(uint16_t n);
	void rti(uint16_t addr);
	void rts(uint16_t addr);
	void sbc(uint16_t addr);
	void sec(uint16_t addr);
	void sed(uint16_t addr);
	void sei(uint16_t addr);
	void sta(uint16_t addr);
	void stx(uint16_t addr);
	void sty(uint16_t addr);
	void tax(uint16_t addr);
	void tay(uint16_t addr);
	void tsx(uint16_t addr);
	void txa(uint16_t addr);
	void txs(uint16_t addr);
	void tya(uint16_t addr);

	void alr(uint16_t addr);
	void anc(uint16_t addr);
	void arr(uint16_t addr);
	void dcp(uint16_t addr);
	void isb(uint16_t addr);
	void las(uint16_t addr);
	void lax(uint16_t addr);
	void lxa(uint16_t addr);
	void rla(uint16_t addr);
	void rra(uint16_t addr);
	void sax(uint16_t addr);
	void sbx(uint16_t addr);
	void sha(uint16_t addr);
	void shx(uint16_t addr);
	void shy(uint16_t addr);
	void slo(uint16_t addr);
	void sre(uint16_t addr);
	void tas(uint16_t addr);

public:
	CPU(bus_read_t bus_read, bus_write_t bus_write);

	void nmi();
	void irq();
	void reset();

	bool halted;

	// returns cycles taken
	size_t step(std::function<void(CPU *)> callback=nullptr);
	void exec(int cycles = 0, std::function<void(CPU *)> callback=nullptr);

	void set_read(bus_read_t bus_read);
	void set_write(bus_write_t bus_write);

	static const uint16_t NMIL = 0xFFFA;
	static const uint16_t NMIH = 0xFFFB;
	static const uint16_t RSTL = 0xFFFC;
	static const uint16_t RSTH = 0xFFFD;
	static const uint16_t IRQL = 0xFFFE;
	static const uint16_t IRQH = 0xFFFF;

	std::string disas(uint8_t instr, uint16_t val);
	void trace();
	int instr_bytes(AddrMode mode);
};
