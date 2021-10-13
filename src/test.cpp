#include <test.hpp>

#include <iostream>

void run_tests(CPU *cpu)
{
	// OVERFLOW FLAG TESTS
	cpu_test(std::vector<uint8_t>({
		0xa9, 0x50,
		0x69, 0x50
	}), [](CPU *cpu) -> bool {
		return !GET_BIT(cpu->sr, CARRY) && GET_BIT(cpu->sr, OV);
	}, cpu);

	cpu_test(std::vector<uint8_t>({
		0xa9, 0x50,
		0x69, 0x90
	}), [](CPU *cpu) -> bool {
		return !GET_BIT(cpu->sr, CARRY) && !GET_BIT(cpu->sr, OV);
	}, cpu);

	cpu_test(std::vector<uint8_t>({
		0xa9, 0x50,
		0x69, 0xd0
	}), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, CARRY) && !GET_BIT(cpu->sr, OV);
	}, cpu);

	cpu_test(std::vector<uint8_t>({
		0xa9, 0xd0,
		0x69, 0x90
	}), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, CARRY) && GET_BIT(cpu->sr, OV);
	}, cpu);

	// INT
	
}

void cpu_test(std::vector<uint8_t> binfile, std::function<bool(CPU*)> verify, CPU *cpu)
{
	uint8_t ram[0x10000];

	int i = 0x400;
	for (char c : binfile) ram[i++] = c;
	
	ram[i] = 0xFF;

	ram[CPU::RSTH] = 4;
	ram[CPU::RSTL] = 0;

	cpu->set_read([&](uint16_t addr) -> uint8_t { return ram[addr]; });
	cpu->set_write([&](uint16_t addr, uint8_t val) { ram[addr] = val; });

	cpu->reset();

	cpu->exec(1000, true);

	printf(verify(cpu) ? "\tTEST PASSED\n" : "\tTEST FAILED\n");
}
