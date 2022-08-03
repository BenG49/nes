#pragma once

#include <cpu.hpp>
#include <rom.hpp>

#include <fstream>

struct NES {
public:
	NES(const std::vector<uint8_t> &ines);

	ROM rom;
	CPU cpu;
	PPU ppu;

	// CPU
	uint8_t ram[0x800];

	// PPU
	uint8_t vram[0x1000];
	uint8_t palette[0x20];

	uint8_t read(uint16_t addr);
	uint8_t read_prg_rom(uint16_t addr);

	void write(uint16_t addr, uint8_t data);

	void run(std::function<void(CPU *)> cpu_callback=nullptr);
};
