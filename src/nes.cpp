#include <nes.hpp>

#include <functional>
#include <iostream>

NES::NES(const std::vector<uint8_t> &ines)
	: rom(ines)
	, cpu([=](uint16_t addr) -> uint8_t { return this->read(addr); },
	      [&](uint16_t addr, uint8_t data) { this->write(addr, data); })
	, ppu(this, rom.mirroring,
		[=](uint16_t addr) -> uint8_t {
			if (addr < 0x2000) return rom.chr_rom[addr];
			else if (addr < 0x3F00) return vram[addr & 0xFFF];
			else if (addr < 0x4000) return palette[addr & 0x1F];
			else {
				printf("Read to unsupported location: 0x%04X\n", addr);
				exit(1);
			}
		},
		[&](uint16_t addr, uint8_t data) {
			if (addr < 0x2000) rom.chr_rom[addr] = data;
			else if (addr < 0x3F00) vram[addr & 0xFFF] = data;
			else if (addr < 0x4000) palette[addr & 0x1F] = data;
			else {
				printf("Read to unsupported location: 0x%04X\n", addr);
				exit(1);
			}
		})
	, ram()
	, vram()
	, palette()
{
	// $C000 is an entry point for incomplete emulators
	// $C004 is ths normal entry point
	rom.prg_rom[(cpu.RSTL - 0x8000) % 0x4000] = 0;
	cpu.reset();
}

uint8_t NES::read(uint16_t addr)
{
	// ram and ram mirrors
	if (addr < 0x2000) {
		return ram[addr & (0x800 - 1)];
	}
	// ppu space
	else if (addr < 0x4000) {
		return ppu.read(addr);
	}
	// prg rom space
	else if (addr >= 0x8000) {
		return read_prg_rom(addr);
	} else {
		printf("Read to unsupported location: 0x%04X\n", addr);
		exit(1);
	}
}

uint8_t NES::read_prg_rom(uint16_t addr)
{
	addr -= 0x8000;

	if (rom.prg_rom.size() == 0x4000) {
		return rom.prg_rom[addr % 0x4000];
	}

	return rom.prg_rom[addr];
}

void NES::write(uint16_t addr, uint8_t data)
{
	// ram and ram mirrors
	if (addr < 0x2000) {
		ram[addr & (0x800 - 1)] = data;
	}
	// ppu
	else if (addr < 0x4000) {
		ppu.write(addr, data);
	}
	// prg rom space
	else if (addr >= 0x8000) {
		perror("Illegal write to cartridge!");
		exit(1);
	} else {
		printf("Write to unsupported location: 0x%04X\n", addr);
		exit(1);
	}
}

void NES::run(std::function<void(CPU *)> cpu_callback) {
	while (true) {
		uint8_t cpu_cycs = cpu.step(cpu_callback);

		ppu.exec(cpu_cycs * 3);
	}
}
