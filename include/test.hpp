#pragma once

#include <cpu.hpp>

#include <vector>

void run_tests(CPU *cpu);

struct MemState {
	uint16_t prog_addr;
	// prog mem starting at prog_addr
	std::vector<uint8_t> binfile;

	uint16_t nmi;
	uint16_t irq;

	MemState(const std::vector<uint8_t> &bin, uint16_t prog_addr)
		: prog_addr(prog_addr), binfile(bin), nmi(), irq() {}
	MemState(const std::vector<uint8_t> &bin, uint16_t prog_addr, uint16_t nmi, uint16_t irq)
		: prog_addr(prog_addr), binfile(bin), nmi(nmi), irq(irq) {}
};

void cpu_test(MemState mem, std::function<bool(CPU*)> verify, CPU *cpu);
