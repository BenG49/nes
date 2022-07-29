#include <ppu.hpp>

#include <nes.hpp>

PPU::PPU(NES *nes, Mirroring mirroring)
	: nes(nes), mirroring(mirroring) {}

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
			uint8_t out = status.get();
			status.in_vblank = false;
			return out;
		}
		// PPUDATA
		else if (addr == 7) {
			uint8_t out = read_buffer;
			read_buffer = internal_read(ppuaddr.addr);

			ppuaddr.inc(ctrl.inc_vram_on_read);
			
			return out;
		}
	}
}

void PPU::write(uint16_t addr, uint8_t data) {
	if (addr >= 0x2000 && addr < 0x4000) {
		addr &= 0b111;

		// PPUCTRL
		if (addr == 0) {
			bool gen_nmi_prev = ctrl.gen_nmi;
			
			ctrl.set(data);

			if (!gen_nmi_prev && ctrl.gen_nmi && status.in_vblank) {
				nes->cpu.nmi();
			}
		}
		// PPUMASK
		else if (addr == 1) { mask.set(data); }
		// OAMADDR
		else if (addr == 3) { oamaddr = data; }
		// PPUSCROLL
		else if (addr == 5) { scroll = data; }
		// PPUADDR
		else if (addr == 6) {
			if (ppuaddr.hi) {
				ppuaddr.addr &= 0xFF00;
				ppuaddr.addr |= data;
			} else {
				ppuaddr.addr &= 0xFF;
				ppuaddr.addr |= (data << 8);

				ppuaddr.hi = true;
			}
		}
		// PPUDATA
		else if (addr == 7) {
			internal_write(ppuaddr.addr, data);
			
			ppuaddr.inc(ctrl.inc_vram_on_read);
		}
	}
	// OAMDMA
	else if (addr == 0x4014) {
		// TODO: implement
	}
}

void PPU::exec(uint8_t cycles) {
	while (cycles--) step();
}

void PPU::step() {
	cycles++;

	if (cycles >= 341) {
		cycles -= 341;
		scanline++;

		if (scanline == 241 && ctrl.gen_nmi) {
			status.in_vblank = true;

			nes->cpu.nmi();	
		} else if (scanline >= 262) {
			scanline = 0;
			status.in_vblank = false;
		}
	}
}
