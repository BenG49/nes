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
public:
	enum Mirroring {
		VERTICAL,
		HORIZONTAL,
		FOUR_SCREEN
	};

private:
	/*
	Pattern table defines the shapes of tiles and sprites
	and is split into two parts - left and right

	Each tile is 16 bytes - two 8 byte bit planes, low
	and high.

	Located from $0000 - $1FFF
	*/
	uint8_t patterntable[2][0x1000];
	uint8_t nametable[0x1000];
	uint8_t palette[0x20];
	uint8_t oam[0x100];

	Mirroring mirroring;

	// PPUCTRL
	uint8_t ctrl;

	// PPUMASK
	uint8_t mask;

	// PPUSTATUS
	uint8_t status;

	// PPUADDR
	uint16_t ppuaddr;
	bool ppuaddr_hi;

	// PPUDATA
	uint8_t read_buffer;

	uint8_t internal_read(uint16_t addr);
	void internal_write(uint16_t addr, uint8_t data);

	uint16_t mirror_nametable_addr(uint16_t addr);
	void inc_ppuaddr();
public:

	PPU();

	// communication from cpu to ppu
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);
};
