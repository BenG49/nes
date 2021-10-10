#pragma once

#include <cpu.hpp>

class Emul {
	CPU *cpu;

	// convert opcode data to address
	addr_t addr(uint8_t *mem) const;
	// convert data in memory to address
	uint16_t get(addr_t addr) const;
	void reljmp(uint8_t jmp);

	void add(cpu_t n);
	void and_f(cpu_t n);
	cpu_t asl(cpu_t n);
	void bit(cpu_t n);
	void cmp(cpu_t n);
	void cpx(cpu_t n);
	void cpy(cpu_t n);
	void eor(cpu_t n);
	void lda(cpu_t n);
	void ldx(cpu_t n);
	void ldy(cpu_t n);
	void ora(cpu_t n);
	void sbc(cpu_t n);
	cpu_t rol(cpu_t n);
	cpu_t ror(cpu_t n);
	void sta(addr_t n);
	void stx(addr_t n);
	void sty(addr_t n);

	void push(cpu_t n);
	cpu_t pop();
	addr_t pop_word();

public:
	Emul(CPU *cpu) : cpu(cpu) {}

	void emulate();
};
