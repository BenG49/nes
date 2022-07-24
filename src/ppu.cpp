#include <ppu.hpp>

PPU::PPU(Mirroring mirroring)
	: mirroring(mirroring) {}

uint16_t PPU::mirror_nametable_addr(uint16_t addr) {
	if (mirroring == Mirroring::FOUR_SCREEN) {
		return addr;
	}

	uint8_t table = (addr >> 10) & 0b11;

	if (mirroring == Mirroring::HORIZONTAL) {
		if (table == 1) table = 0;
		if (table == 3) table = 2;
	} else if (mirroring == Mirroring::VERTICAL) {
		if (table == 2) table = 0;
		if (table == 3) table = 1;
	}

	// get rid of table number
	addr &= 0xF3FF;
	// set table number
	addr |= ((table & 0b11) << 10);

	return addr;
}

void PPU::inc_ppuaddr() {
	if ((ctrl >> 2) & 1) {
		ppuaddr += 32;
	} else {
		ppuaddr += 1;
	}
}

uint8_t PPU::internal_read(uint16_t addr) {
	if (addr < 0x2000) {
		uint8_t table = (addr >> 12) & 1;

		return patterntable[table][addr & 0xFFF];
	} else if (addr < 0x3000) {
		return nametable[mirror_nametable_addr(addr) - 0x2000];
	} else if (addr < 0x2F00) {
		// mirror back down to 0x2000-0x2EFF
		return nametable[mirror_nametable_addr(addr & 0x2FFF) - 0x2000];
	} else if (addr < 0x4000) {
		// 0x3F20-0x3FFF mirrors back to 0x3F00-0x3F1F
		return palette[(addr - 0x3F00) & 0x1F];
	}

	printf("Read to unsupported location on PPU bus: 0x%04X\n", addr);
	exit(1);
}

void PPU::internal_write(uint16_t addr, uint8_t data) {
	if (addr < 0x2000) {
		uint8_t table = (addr >> 12) & 1;

		patterntable[table][addr & 0xFFF] = data;
	} else if (addr < 0x3000) {
		nametable[mirror_nametable_addr(addr) - 0x2000] = data;
	} else if (addr < 0x2F00) {
		// mirror back down to 0x2000-0x2EFF
		nametable[mirror_nametable_addr(addr & 0x2FFF) - 0x2000] = data;
	} else if (addr < 0x4000) {
		// 0x3F20-0x3FFF mirrors back to 0x3F00-0x3F1F
		palette[(addr - 0x3F00) & 0x1F] = data;
	}

	printf("Write to unsupported location on PPU bus: 0x%04X\n", addr);
	exit(1);
}

uint8_t PPU::read(uint16_t addr) {
	if (addr >= 0x2000 && addr < 0x4000) {
		addr &= 0b111;

		// PPUSTATUS
		if (addr == 2) {
			// TODO: implement
			return 0;
		}
		// PPUDATA
		else if (addr == 7) {
			uint8_t out = read_buffer;
			read_buffer = internal_read(ppuaddr);

			inc_ppuaddr();
			
			return out;
		}
	}
}

void PPU::write(uint16_t addr, uint8_t data) {
	if (addr >= 0x2000 && addr < 0x4000) {
		addr &= 0b111;

		// PPUCTRL
		if (addr == 0) { ctrl = data; }
		// PPUMASK
		else if (addr == 1) { mask = data; }
		// OAMADDR
		else if (addr == 3) { oamaddr = data; }
		// PPUSCROLL
		else if (addr == 5) { scroll = data; }
		// PPUADDR
		else if (addr == 6) {
			if (ppuaddr_hi) {
				ppuaddr &= 0xFF00;
				ppuaddr |= data;
			} else {
				ppuaddr &= 0xFF;
				ppuaddr |= (data << 8);

				ppuaddr_hi = true;
			}
		}
		// PPUDATA
		else if (addr == 7) {
			internal_write(ppuaddr, data);
			
			inc_ppuaddr();
		}
	}
	// OAMDMA
	else if (addr == 0x4014) {
		// TODO: implementwa
	}
}
