#include <SDL2/SDL.h>

#include <iostream>
#include <random>
#include <string>

#include <nes.hpp>

uint64_t PALETTE[] = {
   0x808080, 0x003DA6, 0x0012B0, 0x440096, 0xA1005E,
   0xC70028, 0xBA0600, 0x8C1700, 0x5C2F00, 0x104500,
   0x054A00, 0x00472E, 0x004166, 0x000000, 0x050505,
   0x050505, 0xC7C7C7, 0x0077FF, 0x2155FF, 0x8237FA,
   0xEB2FB5, 0xFF2950, 0xFF2200, 0xD63200, 0xC46200,
   0x358000, 0x058F00, 0x008A55, 0x0099CC, 0x212121,
   0x090909, 0x090909, 0xFFFFFF, 0x0FD7FF, 0x69A2FF,
   0xD480FF, 0xFF45F3, 0xFF618B, 0xFF8833, 0xFF9C12,
   0xFABC20, 0x9FE30E, 0x2BF035, 0x0CF0A4, 0x05FBFF,
   0x5E5E5E, 0x0D0D0D, 0x0D0D0D, 0xFFFFFF, 0xA6FCFF,
   0xB3ECFF, 0xDAABEB, 0xFFA8F9, 0xFFABB3, 0xFFD2B0,
   0xFFEFA6, 0xFFF79C, 0xD7E895, 0xA6EDAF, 0xA2F2DA,
   0x99FFFC, 0xDDDDDD, 0x111111, 0x111111
};

struct Window {
	SDL_Window *win;
	SDL_Renderer *renderer;
	SDL_Texture *tex;

	uint32_t *pixels;

	int width, height;

	Window(int w, int h) {
		width = w;
		height = h;

		if (SDL_Init(SDL_INIT_EVERYTHING)) {
			printf("Error initializing SDL: %s\n", SDL_GetError());
		}

		win = SDL_CreateWindow(
			"nes",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width, height, 0);
		
		renderer = SDL_CreateRenderer(win, -1, 0);

		tex = SDL_CreateTexture(
			renderer,
			SDL_PIXELFORMAT_RGB888,
			SDL_TEXTUREACCESS_STATIC,
			width, height);
		
		pixels = (uint32_t *)malloc(width * 4 * height);
	}

	void set_px(int x, int y, uint32_t rgb) {
		if (x > width || y > height) {
			perror("Attempted to set out of bounds pixel\n");
			return;
		}

		pixels[y * width + x] = rgb;
	}

	void render() {
		bool run = true;
		
		while (run) {
			SDL_RenderClear(renderer);
			SDL_UpdateTexture(tex, nullptr, pixels, width * 4);
			SDL_RenderCopy(renderer, tex, nullptr, nullptr);
			SDL_RenderPresent(renderer);

			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
					case SDL_QUIT:
						run = false;
						break;
				}
			}
		}
	}
};

void get_tile(PPU *ppu, uint8_t bank, uint8_t x, uint8_t y, Window *out) {
	// each tile is 16 bytes, 16x16 tiles
	uint16_t addr = 0x1000 * bank + x * 16 + y * 128;

	for (int py = 0; py < 8; py++) {
		for (int px = 0; px < 8; px++) {
			uint8_t val = 
				((ppu->internal_read(addr + py) >> (7 - px)) & 1) | 
				(((ppu->internal_read(addr + py + 8) >> (7 - px)) & 1) << 1);

			switch (val) {
				case 0: out->set_px(x * 10 + px + bank * 170, y * 10 + py, PALETTE[0x01]); break;
				case 1: out->set_px(x * 10 + px + bank * 170, y * 10 + py, PALETTE[0x23]); break;
				case 2: out->set_px(x * 10 + px + bank * 170, y * 10 + py, PALETTE[0x27]); break;
				case 3: out->set_px(x * 10 + px + bank * 170, y * 10 + py, PALETTE[0x30]); break;
			}
		}
	}
}

int main(int argc, const char *argv[])
{
	Window win(512, 240);

	NES nes(readfile("pacman.nes"));

	for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 16; x++) {
			get_tile(&nes.ppu, 0, x, y, &win);
			get_tile(&nes.ppu, 1, x, y, &win);
		}
	}

	// nes.run(&CPU::trace);

	win.render();
	// nes.cpu.halted = true;
}
