#pragma once

#include <util.hpp>

const uint8_t PPUCTRL = 0;
const uint8_t PPUMASK = 1;
const uint8_t PPUSTATUS = 2;
const uint8_t OAMADDR = 3;
const uint8_t OAMDATA = 4;
const uint8_t PPUSCROLL = 5;
const uint8_t PPUADDR = 6;
const uint8_t PPUDATA = 7;

class PPU {
private:
	/*
	Pattern table defines the shapes of tiles and sprites
	and is split into two parts - left and right

	Each tile is 16 bytes - two 8 byte bit planes, low
	and high.

	Located from $0000 - $1FFF
	*/
	uint8_t patterntable[0x2000];
	uint8_t nametable[0x1000];
	uint8_t palette[0x20];

	struct Addr {
		uint16_t addr;
		bool wrote_hi;
	};

	Addr ppuaddr;

	uint8_t ppu_read(uint8_t addr);
	uint8_t ppu_write(uint8_t addr, uint8_t val);

public:
	enum Mirroring {
		VERTICAL,
		HORIZONTAL,
		FOUR_SCREEN
	};

	PPU();

	uint8_t read(uint8_t addr);
	uint8_t write(uint8_t addr, uint8_t val);
};
