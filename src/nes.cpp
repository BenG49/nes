#include <nes.hpp>

#include <iostream>

#define HDR_BYTE(byte, err) \
	in >> inb;              \
	if (inb != byte)        \
		read_err(#err);

// TODO: add mapper or sth

NES::NES(std::ifstream &ines)
	: cpu([=](uint16_t addr) -> uint8_t {
		// ram and mirrors
		if (addr < 0x2000)
			return ram[addr & RAM_MASK];
		// cartridge
		else if (addr > 0x4020)
			return rom[addr - 0x4020];
		else
		{
			printf("Read to unsupported location: 0x%04X\n", addr);
			exit(1);
		}
	}, [&](uint16_t addr, uint8_t n) -> void {
		// printf("[0x%04X] = 0x%02X\n", addr, n);
		if (addr < 0x2000)
			ram[addr & RAM_MASK] = n;
		// cartridge
		else if (addr > 0x4020)
			rom[addr - 0x4020] = n;
		else
		{
			printf("Write to unsupported location: 0x%04X\n", addr);
			exit(1);
		}
	})
{
	read_ines(ines);
}

void NES::read_err(const char *err) const
{
	std::cerr << err << '\n';
	exit(1);
}

void NES::read_ines(std::ifstream &in)
{
	uint8_t inb, prgrom_16k_sz, chrrom_8k_sz;

	HDR_BYTE('N', Invalid ines header!)
	HDR_BYTE('E', Invalid ines header!)
	HDR_BYTE('S', Invalid ines header!)
	HDR_BYTE(0x1A, Invalid ines header!)

	in >> prgrom_16k_sz;
	in >> chrrom_8k_sz;

	in >> inb; // flags 6
	in >> inb; // flags 7
	in >> inb; // flags 8
	in >> inb; // flags 9
	in >> inb; // flags 10

	// read prg rom
	for (int i = prgrom_16k_sz * 0x4000 - 1; i >= 0; --i) in >> rom[i];
}
