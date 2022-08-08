#include <ppu.hpp>

#include <nes.hpp>

PPU::PPU(NES *nes, Mirroring mirroring, bus_read_t internal_read, bus_write_t internal_write)
	: nes(nes), win(256, 240), mirroring(mirroring), internal_read(internal_read), internal_write(internal_write) {}

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
				render();
				win.render();

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

			render();
			win.render();

			nes->cpu.nmi();	
		} else if (scanline >= 262) {
			scanline = 0;
			status.in_vblank = false;
		}
	}
}

void PPU::render() {
	for (int y = 0; y < 30; y++) {
		for (int x = 0; x < 32; x++) {
			uint16_t nametable_addr = 0x2000 + y * 32 + x;
			uint8_t tile = internal_read(nametable_addr);

			uint16_t pattern_addr = 0x1000 * ctrl.bg_addr + tile * 16;

			
			for (int py = 0; py < 8; py++) {
				uint8_t low = internal_read(pattern_addr + py);
				uint8_t high = internal_read(pattern_addr + py + 8);

				for (int px = 0; px < 8; px++) {
					uint8_t val = 
						((low >> (7 - px)) & 1) | 
						(((high >> (7 - px)) & 1) << 1);

					switch (val) {
						case 0: win.set_px(x * 8 + px, y * 8 + py, palette[0x01]); break;
						case 1: win.set_px(x * 8 + px, y * 8 + py, palette[0x23]); break;
						case 2: win.set_px(x * 8 + px, y * 8 + py, palette[0x27]); break;
						case 3: win.set_px(x * 8 + px, y * 8 + py, palette[0x30]); break;
					}
				}
			}
		}
	}
}
