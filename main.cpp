#include <SDL2/SDL.h>

#include <iostream>
#include <random>
#include <string>

#include <nes.hpp>
#include <test.hpp>

void snake()
{
	// -- RAND -- //
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, 0xff);

	// -- SDL -- //
	if (SDL_Init(SDL_INIT_EVERYTHING))
		printf("error initializing SDL: %s\n", SDL_GetError());

	SDL_Window *win = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 32, 32, 0);
	SDL_Surface *surface = SDL_GetWindowSurface(win);
	Uint32 *pixels = (Uint32 *)surface->pixels;

	Uint32 colors[] = {
		SDL_MapRGBA(surface->format, 0,   0,   0,   255),
		SDL_MapRGBA(surface->format, 255, 255, 255, 255),
		SDL_MapRGBA(surface->format, 100, 100, 100, 255),
		SDL_MapRGBA(surface->format, 255, 0,   0,   255),
		SDL_MapRGBA(surface->format, 0,   255, 0,   255),
		SDL_MapRGBA(surface->format, 0,   0,   255, 255),
		SDL_MapRGBA(surface->format, 255, 0,   255, 255),
		SDL_MapRGBA(surface->format, 255, 255, 0,   255),
		SDL_MapRGBA(surface->format, 0,   255, 255, 255),
	};

	// -- CPU -- //
	CPU cpu([](uint16_t n){ return 0; }, [](uint16_t a, uint8_t n) {});

	uint8_t ram[0x10000];

	std::vector<uint8_t> binfile = readfile("snake.bin");
	int i = 0x600;
	for (uint8_t c : binfile) ram[i++] = c;
	
	ram[i] = 0xFF;

	ram[CPU::RSTH] = MSB(0x600);
	ram[CPU::RSTL] = LSB(0x600);

	ram[CPU::IRQH] = MSB(0);
	ram[CPU::IRQL] = LSB(0);
	ram[CPU::NMIH] = MSB(0);
	ram[CPU::NMIL] = LSB(0);

	cpu.set_read([&](uint16_t addr) -> uint8_t {
		if (addr == 0xfe) {
			return (uint8_t)dist(rng);
		}

		return ram[addr];
	});
	cpu.set_write([&](uint16_t addr, uint8_t val) {
		ram[addr] = val;

		if (addr >= 0x200 && addr < 0x600)
		{
			pixels[addr - 0x200] = colors[val];
		}
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

		SDL_UpdateWindowSurface(win);
	}

	SDL_DestroyWindow(win);
	SDL_Quit();
}

void step(CPU *cpu)
{
	cpu->reset();

	bool run = true;
	while (run)
	{
		std::string s;
		s = std::cin.get();

		switch (s.at(0))
		{
			// exec instr
			case 'n':
			case '\n':
				cpu->exec(1, true);
				break;
			// print zeropage
			case 'z':
				for (int i = 0; i < 0x100; ++i)
				{
					if (i % 0x10 == 0)
						printf("\n\t");
					printf("%02X ", cpu->bus_read(i));
				}
				printf("\n");
				break;
			case 'q':
				run = false;
				break;
		}
	}
}

/*
0000: 00 02 02 04 00 00 00 00
0008: 00 00 00 00 00 00 00 00
0010: 11 04 10 04 0F 04 00 00
*/

// TODO: fix BEQ extra byte when jumping
int main(int argc, const char *argv[])
{
	puts("addr instr     disass        |AC XR YR SP|nvdizc");
	snake();

	// cpu_test(MemState(readfile("snake.bin"), 0x600), [](CPU *cpu) -> bool { return true; });

	// run_tests();

	/*std::ifstream in(argv[1], std::ios::binary);

	NES nes(in);

	std::string s;
	while (true)
	{
		printf("> ");
		std::getline(std::cin, s);

		switch (s[0]) {
			case 'q': exit(0);
			case 'x': {
				uint16_t addr = std::strtol(s.c_str() + 2, nullptr, 16);
				printf("0x%04X: %02X\n", addr, cpu.bus_read(addr));
				break;
			}
			case 'r':
				while (true)
					cpu.exec(1, true);
				break;
			default:
				cpu.exec(1, true);
				break;
		}
	}*/
}
