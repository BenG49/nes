#pragma once

#include <vector>
#include <cstdint>

#include <ppu.hpp>

class ROM {
private:
	static const int PRG_ROM_PAGE_SIZE = 0x4000;
	static const int CHR_ROM_PAGE_SIZE = 0x2000;

public:
	std::vector<uint8_t> prg_rom;
	std::vector<uint8_t> chr_rom;
	uint8_t mapper;
	PPU::Mirroring mirroring;

	ROM(const std::vector<uint8_t> &ines);
};
