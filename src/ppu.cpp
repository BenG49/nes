#include <ppu.hpp>

uint8_t PPU::ppu_read(uint8_t addr) {
	if (addr < 0x2000) {
		// uint8_t 
	}
}

uint8_t PPU::read(uint8_t addr) {
	if (addr >= 0x2000 && addr < 0x4000) {
		addr &= 0b111;
	}
}

uint8_t PPU::write(uint8_t addr, uint8_t val) {
	if (addr >= 0x2000 && addr < 0x4000) {
		addr &= 0b111;

		// PPUADDR
		if (addr == 6) {
			if (ppuaddr.wrote_hi) {
				ppuaddr.addr &= 0xFF00;
				ppuaddr.addr |= val;
			} else {
				ppuaddr.addr &= 0xFF;
				ppuaddr.addr |= (val << 8);

				ppuaddr.wrote_hi = true;
			}
		}
	}
}
