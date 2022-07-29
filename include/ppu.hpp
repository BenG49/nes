#pragma once

#include <util.hpp>

class NES;

const uint8_t PPUCTRL = 0;
const uint8_t PPUMASK = 1;
const uint8_t PPUSTATUS = 2;
const uint8_t OAMADDR = 3;
const uint8_t OAMDATA = 4;
const uint8_t PPUSCROLL = 5;
const uint8_t PPUADDR = 6;
const uint8_t PPUDATA = 7;

struct Ctrl {
	uint8_t base_addr;
	bool inc_vram_on_read;
	bool sprite_addr;
	bool bg_addr;
	bool sprite_size;
	bool master_slave_sel;
	bool gen_nmi;

	uint8_t get() {
		return (base_addr & 0b11)
			| (inc_vram_on_read << 2)
			| (sprite_addr << 3)
			| (bg_addr << 4)
			| (sprite_size << 5)
			| (master_slave_sel << 6)
			| (gen_nmi << 7);
	}

	void set(uint8_t val) {
		base_addr = val & 0b11;
		inc_vram_on_read = GET_BIT(val, 2);
		sprite_addr =      GET_BIT(val, 3);
		bg_addr =          GET_BIT(val, 4);
		sprite_size =      GET_BIT(val, 5);
		master_slave_sel = GET_BIT(val, 6);
		gen_nmi =          GET_BIT(val, 7);
	}

	Ctrl() {}
};

struct Mask {
	bool greyscale;
	bool show_left_bg;
	bool show_left_sprite;
	bool show_bg;
	bool show_sprite;
	bool tint_red;
	bool tint_green;
	bool tint_blue;

	uint8_t get() {
		return greyscale
			| (show_left_bg << 1)
			| (show_left_sprite << 2)
			| (show_bg << 3)
			| (show_sprite << 4)
			| (tint_red << 5)
			| (tint_green << 6)
			| (tint_blue << 7);
	}

	void set(uint8_t val) {
		greyscale =        GET_BIT(val, 0);
		show_left_bg =     GET_BIT(val, 1);
		show_left_sprite = GET_BIT(val, 2);
		show_bg =          GET_BIT(val, 3);
		show_sprite =      GET_BIT(val, 4);
		tint_red =         GET_BIT(val, 5);
		tint_blue =        GET_BIT(val, 6);
		tint_green =       GET_BIT(val, 7);
	}

	Mask() {}
};

// TODO: add PPU open bus
struct Status {
	bool sprite_ov, sprite_zero_hit, in_vblank;

	uint8_t get() {
		return (sprite_ov << 5)
			| (sprite_zero_hit << 6)
			| (in_vblank << 7);
	}

	Status() {}
};

struct Addr {
	uint16_t addr;
	bool hi;

	void inc(bool ctrl_inc) {
		if (ctrl_inc) {
			addr += 32;
		} else {
			addr += 1;
		}
	}

	Addr() {};
};


class PPU {
public:
	enum Mirroring {
		VERTICAL,
		HORIZONTAL,
		FOUR_SCREEN
	};

private:
	NES *nes;

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

	// PPUCTRL (0x2000)
	Ctrl ctrl;
	// PPUMASK (0x2001)
	Mask mask;
	// PPUSCROLL (0x2005)
	uint8_t scroll;
	// OAMADDR (0x2003)
	uint8_t oamaddr;

	Status status;

	// PPUADDR (0x2006)
	Addr ppuaddr;

	uint8_t read_buffer;

	uint8_t internal_read(uint16_t addr);
	void internal_write(uint16_t addr, uint8_t data);

	uint16_t mirror_nametable_addr(uint16_t addr);


	uint16_t scanline;
	size_t cycles;

public:
	PPU(NES *nes, Mirroring mirroring);

	// communication from cpu to ppu
	uint8_t read(uint16_t addr);
	void write(uint16_t addr, uint8_t data);

	void exec(uint8_t cycles = 0);
	void step();
};
