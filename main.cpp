#include <SDL2/SDL.h>

#include <iostream>
#include <random>
#include <string>

#include <nes.hpp>
#include <test.hpp>

// https://benedicthenshaw.com/soft_render_sdl2.html
void snake()
{
	// -- RAND -- //
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(1, 0xf);

	// -- SDL -- //
	if (SDL_Init(SDL_INIT_EVERYTHING))
		printf("error initializing SDL: %s\n", SDL_GetError());

	SDL_Window *win = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, 0);

	SDL_SetWindowMinimumSize(win, 32, 32);

	SDL_RenderSetLogicalSize(renderer, 32, 32);
	SDL_RenderSetIntegerScale(renderer, (SDL_bool)true);

	SDL_Texture *screen_tex = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGB888,
		SDL_TEXTUREACCESS_STATIC,
		32, 32
	);

	uint32_t *pixels = (uint32_t *)malloc(32 * 32 * 4);

	uint32_t colors[] = {
		0x000000, // black
		0xffffff, // white
		0x404040, // gray
		0xff0000, // red
		0x00ff00, // green
		0x0000ff, // blue
		0x00ffff, // magenta
		0xff00ff, // yellow
		0xffff00, // cyan
		0x404040, // gray
		0xff0000, // red
		0x00ff00, // green
		0x0000ff, // blue
		0x00ffff, // magenta
		0xff00ff, // yellow
		0xffff00, // cyan
	};

	for (int i = 0; i < 32 * 32; ++i)
		pixels[i] = 0;

	// -- CPU -- //
	CPU cpu([](uint16_t n){ return 0; }, [](uint16_t a, uint8_t n) {});

	uint8_t ram[0x10000];

	std::vector<uint8_t> binfile = readfile("snake.bin");
	int i = 0x600;
	for (uint8_t c : binfile) ram[i++] = c;
	
	ram[i] = 0xFF;

	ram[CPU::RSTH] = 6;
	ram[CPU::RSTL] = 0;

	cpu.set_read([&](uint16_t addr) -> uint8_t {
		if (addr == 0xfe)
			return (uint8_t)dist(rng);

		return ram[addr];
	});
	cpu.set_write([&](uint16_t addr, uint8_t val) {
		ram[addr] = val;

		if (addr >= 0x200 && addr < 0x600)
			pixels[addr - 0x200] = colors[val];
	});

	cpu.reset();

	bool run = true;

	while (run)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type) {
				case SDL_QUIT:
					run = false;
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
						case SDLK_w:
							cpu.bus_write(0xff, 0x77);
							break;
						case SDLK_s:
							cpu.bus_write(0xff, 0x73);
							break;
						case SDLK_a:
							cpu.bus_write(0xff, 0x61);
							break;
						case SDLK_d:
							cpu.bus_write(0xff, 0x64);
							break;
					}

					break;
			}
		}
		
		cpu.exec(1, true);

		SDL_RenderClear(renderer);
		SDL_UpdateTexture(screen_tex, nullptr, pixels, 32 * 4);
		SDL_RenderCopy(renderer, screen_tex, nullptr, nullptr);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(win);
	SDL_Quit();
}

// TODO: fix BEQ extra byte when jumping
int main(int argc, const char *argv[])
{
	puts("addr instr     disass        |AC XR YR SP|nvdizc");
	snake();

	// run_tests();
}
