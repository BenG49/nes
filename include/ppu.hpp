#pragma once

/*

PPU:
- 256x240
	- 64 sprites
	- scrollable field

*/

class PPU {
public:
	enum Mirroring {
		VERTICAL,
		HORIZONTAL,
		FOUR_SCREEN
	};

	PPU();
};
