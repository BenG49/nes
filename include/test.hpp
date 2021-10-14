#pragma once

#include <cpu.hpp>

#include <vector>

void run_tests(CPU *cpu);

struct MemState {
	// prog mem starting at 0x400
	std::vector<uint8_t> binfile;

	uint16_t nmi;
	uint16_t irq;

	MemState(const std::vector<uint8_t> &bin)
		: binfile(bin), nmi(), irq() {}
	MemState(const std::vector<uint8_t> &bin, uint16_t nmi, uint16_t irq)
		: binfile(bin), nmi(nmi), irq(irq) {}
};

void cpu_test(MemState *mem, std::function<bool(CPU*)> verify, CPU *cpu);
