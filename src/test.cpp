#include <test.hpp>

#include <fstream>

bool cpu_test(std::ifstream &test, int instrs, std::function<bool(CPU&)> verify)
{
	uint8_t ram[0x10000];

	int i = 0x400;
	while (test.read((char *)ram + i++, 1));
	
	ram[i - 1] = 0xFF;

	ram[CPU::RSTH] = 4;
	ram[CPU::RSTL] = 0;

	CPU cpu(
		[&](uint16_t addr) -> uint8_t { return ram[addr]; },
		[&](uint16_t addr, uint8_t val) { ram[addr] = val; }
	);

	cpu.exec(instrs, true);

	return verify(cpu);
}
