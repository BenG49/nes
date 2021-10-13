#pragma once

#include <cpu.hpp>

#include <fstream>

struct NES {
private:
	static const int RAM_SIZE = 0x800;
	static const int RAM_MASK = 0x7FF;

	void read_err(const char *err) const;
	void read_ines(std::ifstream &in);

public:
	CPU cpu;

	// 0x0000-0x07FF
	uint8_t ram[RAM_SIZE];

	// mirrors of ram
	// 0x0800-0x0FFF
	// 0x1000-0x17FF
	// 0x1800-0x1FFF

	// 0x2000-0x2007
	// ppu regs

	// mirror of ppu regs, repeat every 8 bytes
	// 0x2008-0x3FFF

	// apu and i/o regs
	// 0x4000-0x4017

	// apu and i/o normally disabled
	// 0x4018-0x401F

	// cartridge
	// 0x4020-0xFFFF
	uint8_t rom[0xBFDF];

	NES(std::ifstream &ines);
};
