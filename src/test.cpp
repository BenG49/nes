#include <test.hpp>

#include <iostream>

void run_tests(CPU *cpu)
{
	// OVERFLOW FLAG TESTS
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x50,
		0x69, 0x50
	})), [](CPU *cpu) -> bool {
		return !GET_BIT(cpu->sr, CARRY) && GET_BIT(cpu->sr, OV);
	}, cpu);

	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x50,
		0x69, 0x90
	})), [](CPU *cpu) -> bool {
		return !GET_BIT(cpu->sr, CARRY) && !GET_BIT(cpu->sr, OV);
	}, cpu);

	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x50,
		0x69, 0xd0
	})), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, CARRY) && !GET_BIT(cpu->sr, OV);
	}, cpu);

	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0xd0,
		0x69, 0x90
	})), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, CARRY) && GET_BIT(cpu->sr, OV);
	}, cpu);

	// BRK test
	std::vector<uint8_t> v;
	v.resize(0x102);
	v[1] = 0xea;
	v[2] = 0xff;
	v[0x100] = 0x28;
	v[0x101] = 0x60;

	cpu_test(MemState(v, 0, 0x500),
		[](CPU *cpu) -> bool {
			return true;
	}, cpu);
}

void cpu_test(MemState mem, std::function<bool(CPU*)> verify, CPU *cpu)
{
	uint8_t ram[0x10000];

	int i = 0x400;
	for (char c : mem.binfile) ram[i++] = c;
	
	ram[i] = 0xFF;

	ram[CPU::RSTH] = 4;
	ram[CPU::RSTL] = 0;

	ram[CPU::IRQH] = MSB(mem.irq);
	ram[CPU::IRQL] = LSB(mem.irq);
	ram[CPU::NMIH] = MSB(mem.nmi);
	ram[CPU::NMIL] = LSB(mem.nmi);

	cpu->set_read([&](uint16_t addr) -> uint8_t { return ram[addr]; });
	cpu->set_write([&](uint16_t addr, uint8_t val) { ram[addr] = val; });

	cpu->reset();

	cpu->exec(1000, true);

	printf(verify(cpu) ? "\tTEST PASSED\n" : "\tTEST FAILED\n");
}
