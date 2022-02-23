#include <rom.hpp>

#include <stdio.h>
#include <stdlib.h>

inline void err(const char *err)
{
	perror(err);
	exit(-1);
}

ROM::ROM(const std::vector<uint8_t> &ines)
{
	if (ines[0] != 0x4E || ines[1] != 0x45 || ines[2] != 0x53 || ines[3] != 0x1A)
		err("File not in iNES format");

	if ((ines[7] & 0b1100) == 2)
		err("iNES 2.0 unsupported");
	// TODO: check for archaic iNES

	mapper = (ines[6] >> 4) | (ines[7] & 0b11110000);
	
	if (mapper != 0)
		err("Only mapper 0 supported");

	if (ines[6] & 0b1000)
		mirroring = FOUR_SCREEN;
	else if (ines[6] & 1)
		mirroring = VERTICAL;
	else
		mirroring = HORIZONTAL;

	size_t prg_rom_size = ines[4] * PRG_ROM_PAGE_SIZE;
	size_t chr_rom_size = ines[5] * CHR_ROM_PAGE_SIZE;

	bool trainer_present = (ines[6] & 0b100) != 0;

	size_t prg_rom_start = 16 + ((trainer_present) ? 512 : 0);
	size_t chr_rom_start = prg_rom_start + prg_rom_size;

	prg_rom = std::vector<uint8_t>(ines.begin() + prg_rom_start, ines.begin() + prg_rom_start + prg_rom_size);
	chr_rom = std::vector<uint8_t>(ines.begin() + chr_rom_start, ines.begin() + chr_rom_start + chr_rom_size);
}
