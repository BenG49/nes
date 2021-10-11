#pragma once

#include <functional>
#include <cstdint>
#include <string>

#include <util.hpp>

const uint8_t NEG   = 8;
const uint8_t OV    = 7;
const uint8_t CONST = 5;
const uint8_t BRK   = 4;
const uint8_t DEC   = 3;
const uint8_t INT   = 2;
const uint8_t ZERO  = 1;
const uint8_t CARRY = 0;

class CPU;

typedef std::function<uint8_t(uint16_t)> bus_read_t;
typedef std::function<void(uint16_t, uint8_t)> bus_write_t;
typedef void (CPU::*func_t)(uint16_t);

enum AddrMode {
	ACC, IMM, ZPG, ZPX, ZPY, ABS, ABX, ABY, IMPL, REL, IND, INX, INY
};

class CPU {
	// NOTE: stack pointer = 0x100 + sp, grows downward
	// NV-BDIZC
	uint8_t a, x, y, sp, sr;

	uint16_t pc;

	int cycles;
	
	struct Instr {
		func_t f;
		AddrMode a;
	
		uint8_t cycles;

		std::string s;
	
		Instr() : f(&CPU::nop), a(IMPL), cycles() {}
		Instr(func_t f, AddrMode a, uint8_t cycles, const char *str) : f(f), a(a), cycles(cycles), s(str)
		{
			s = s.substr(6, s.length() - 6);
		}
	};

	Instr vec[256];

	bus_read_t bus_read;
	bus_write_t bus_write;
	
	void set_flags(uint16_t val, bool neg, bool zero, bool carry);

	inline void push(uint8_t n);
	inline void push_word(uint16_t n);
	inline uint8_t pop();
	inline uint16_t pop_word();

	void adc(uint16_t addr);
	void _and(uint16_t addr);
	void asl(uint16_t addr); void asl_acc();
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
	void lsr(uint16_t addr); void lsr_acc();
	void nop(uint16_t addr);
	void ora(uint16_t addr);
	void pha(uint16_t addr);
	void php(uint16_t addr);
	void pla(uint16_t addr);
	void plp(uint16_t addr);
	void rol(uint16_t addr); void rol_acc();
	void ror(uint16_t addr); void ror_acc();
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

public:
	CPU(bus_read_t bus_read, bus_write_t bus_write);

	void nmi();
	void irq();
	void reset();

	void exec(int ticks, bool countInstr);

	static const uint16_t NMIL = 0xFFFA;
	static const uint16_t NMIH = 0xFFFB;
	static const uint16_t RSTL = 0xFFFC;
	static const uint16_t RSTH = 0xFFFD;
	static const uint16_t IRQL = 0xFFFE;
	static const uint16_t IRQH = 0xFFFF;
};
