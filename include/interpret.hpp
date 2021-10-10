#pragma once

#include <functional>
#include <fstream>

#include <cpu.hpp>

class Interpret {
	CPU *cpu;

	// convert opcode data to address
	addr_t addr(uint8_t *mem) const;
	// convert data in memory to address
	uint16_t get(addr_t addr) const;

	void add_f(cpu_t n);
	void and_f(cpu_t n);
	uint8_t asl_f(cpu_t n);
	void cmp_f(cpu_t n);
	void eor_f(cpu_t n);
	void lda_f(cpu_t n);
	void ora_f(cpu_t n);
	void sbc_f(cpu_t n);

public:
	Interpret(CPU *cpu)
		: cpu(cpu)
	{
		// TODO: make this better

		cpu->mem[PRGM_START + 0] = 0xea;
	}

	void interpret();
};
