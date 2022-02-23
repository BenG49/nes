#include <test.hpp>

#include <iostream>

void run_tests()
{
	// lda test
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x05, 0x00
	}), 0), [](CPU *cpu) -> bool {
		return cpu->a == 0x05 && GET_BIT(cpu->sr, ZERO) == 0 && GET_BIT(cpu->sr, OV) == 0;
	});

	// lda, tax, inx, brk test
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0xc0, 0xaa, 0xe8, 0x00
	}), 0), [](CPU *cpu) -> bool {
		return cpu->a == 0xc0 && cpu->x == 0xc1;
	});
	
	// ZPG, X test
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x69, // LDA #$69
		0x85, 0x12, // STA $12
		0xa2, 0x02, // LDX #$02
		0xb5, 0x10  // LDA $10, X
	}), 0x400), [](CPU *cpu) -> bool {
		return cpu->a == 0x69;
	});

	std::cout << "---- ADC tests -----\n";
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x50, // LDA #$50
		0x69, 0x50, // ADC #$50
	}), 0x400), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 1 && GET_BIT(cpu->sr, CARRY) == 0;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x50, // LDA #$50
		0x69, 0x90, // ADC #$90
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 0 && GET_BIT(cpu->sr, CARRY) == 0;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x50, // LDA #$50
		0x69, 0xd0, // ADC #$d0
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 0 && GET_BIT(cpu->sr, CARRY) == 1;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0xd0, // LDA #$d0
		0x69, 0x10, // ADC #$10
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 0 && GET_BIT(cpu->sr, CARRY) == 0;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0xd0, // LDA #$d0
		0x69, 0x50, // ADC #$50
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 0 && GET_BIT(cpu->sr, CARRY) == 1;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0xd0, // LDA #$d0
		0x69, 0x90, // ADC #$90
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 1 && GET_BIT(cpu->sr, CARRY) == 1;
	});

	std::cout << "---- SBC tests ----\n";
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x50, // LDA #$50
		0xe9, 0xb0, // SBC #$b0
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 1 && GET_BIT(cpu->sr, CARRY) == 0;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x50, // LDA #$50
		0xe9, 0x70, // SBC #$70
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 0 && GET_BIT(cpu->sr, CARRY) == 0;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0x50, // LDA #$50
		0xe9, 0x30, // SBC #$30
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 0 && GET_BIT(cpu->sr, CARRY) == 1;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0xd0, // LDA #$d0
		0xe9, 0xf0, // SBC #$f0
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 0 && GET_BIT(cpu->sr, CARRY) == 0;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0xd0, // LDA #$d0
		0xe9, 0xb0, // SBC #$b0
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 0 && GET_BIT(cpu->sr, CARRY) == 1;
	});
	cpu_test(MemState(std::vector<uint8_t>({
		0xa9, 0xd0, // LDA #$d0
		0xe9, 0x70, // SBC #$70
	}), 0), [](CPU *cpu) -> bool {
		return GET_BIT(cpu->sr, OV) == 1 && GET_BIT(cpu->sr, CARRY) == 1;
	});

	// BRK test
	/*std::vector<uint8_t> v;
	v.resize(0x102);
	v[1] = 0xea;
	v[2] = 0xff;
	v[0x100] = 0x28;
	v[0x101] = 0x60;

	cpu_test(MemState(v, 0x400, 0, 0x500),
		[](CPU *cpu) -> bool {
			return true;
	});*/
}

void cpu_test(MemState mem, std::function<bool(CPU*)> verify)
{
	uint8_t ram[0x10000];

	int i = mem.prog_addr;
	for (char c : mem.binfile) ram[i++] = c;
	ram[i] = 0x00;
	
	ram[CPU::RSTH] = MSB(mem.prog_addr);
	ram[CPU::RSTL] = LSB(mem.prog_addr);

	ram[CPU::IRQH] = MSB(mem.irq);
	ram[CPU::IRQL] = LSB(mem.irq);
	ram[CPU::NMIH] = MSB(mem.nmi);
	ram[CPU::NMIL] = LSB(mem.nmi);

	CPU cpu(
		[&](uint16_t addr) -> uint8_t { return ram[addr]; },
		[&](uint16_t addr, uint8_t data) { ram[addr] = data; }
	);

	cpu.reset();

	cpu.exec();

	printf(verify(&cpu) ? "\tTEST PASSED\n" : "\tTEST FAILED\n");
}
