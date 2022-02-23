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

	std::function<void(CPU *)> snake_callback = [&](CPU *cpu) {
		SDL_RenderClear(renderer);
		SDL_UpdateTexture(screen_tex, nullptr, pixels, 32 * 4);
		SDL_RenderCopy(renderer, screen_tex, nullptr, nullptr);
		SDL_RenderPresent(renderer);

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type) {
				case SDL_QUIT:
					cpu->halted = true;
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym) {
						case SDLK_w:
							cpu->bus_write(0xff, 0x77);
							break;
						case SDLK_s:
							cpu->bus_write(0xff, 0x73);
							break;
						case SDLK_a:
							cpu->bus_write(0xff, 0x61);
							break;
						case SDLK_d:
							cpu->bus_write(0xff, 0x64);
							break;
					}

					break;
			}
		}
	};

	cpu.exec_with_callback(snake_callback);

	SDL_DestroyWindow(win);
	SDL_Quit();
}

// TODO: add masswerk virtual 6502 trace style
int main(int argc, const char *argv[])
{
	//C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD
	std::function<void(CPU *)> test_trace_f = [](CPU *cpu) {
		uint8_t op = cpu->bus_read(cpu->pc);
		CPU::Instr instr = cpu->vec[op];

		printf("%04X  ", cpu->pc);

		int bytes = cpu->instr_bytes(instr.addr_mode);
		for (int i = 0; i < 2; ++i)
		{
			if (i < bytes)
				printf("%02X ", cpu->bus_read(cpu->pc + i));
			else
				printf("   ");
		}

		uint16_t args = 0;
		if (bytes == 2)
			args = cpu->bus_read(cpu->pc + 1);
		else if (bytes == 3)
			args = cpu->bus_read(cpu->pc + 1) | (cpu->bus_read(cpu->pc + 2) << 8);

		printf(" %-32s A:%02X X:%02X Y:%02X P:%02X SP:%02X", cpu->disas(op, args).c_str(), cpu->a, cpu->x, cpu->y, cpu->sr, cpu->sp);

		puts("");
	};

	NES nes(readfile("nestest.nes"));
	
	/*for (int i = 0; i < 0x100; ++i)
	{
		if (i % 0x10 == 0) printf("\n");

		printf("%02X ", nes.ram[i]);
	}
	printf("\n");*/

	nes.cpu.pc = 0xC000;
	nes.cpu.exec_with_callback(test_trace_f);
}
