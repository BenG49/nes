#include <iostream>

#include <nes.hpp>

int main(int argc, const char *argv[])
{
	/*uint8_t ram[0x10000];

	std::ifstream in("prgm", std::ios::binary);

	int i = 0x300;
	while (in.read((char *)ram + i++, 1));
	
	ram[i - 1] = 0xFF;

	ram[CPU::IRQL] = 3;
	ram[CPU::IRQH] = 0;

	CPU cpu(
		[&](uint16_t addr) -> uint8_t { return ram[addr]; },
		[&](uint16_t addr, uint8_t val) { ram[addr] = val; }
	);*/

	std::ifstream in(argv[1], std::ios::binary);
	
	NES nes(in);

	printf("RST VECTOR 0x%02X%02X\n", nes.cpu.bus_read(nes.cpu.RSTH), nes.cpu.bus_read(nes.cpu.RSTL));

	std::string s;
	while (true)
	{
		printf("> ");
		std::getline(std::cin, s);

		switch (s[0]) {
			case 'q': exit(0);
			case 'x': {
				uint16_t addr = std::strtol(s.c_str() + 2, nullptr, 16);
				printf("0x%04X: %02X\n", addr, nes.cpu.bus_read(addr));
				break;
			}
			case 'r':
				while (true)
					nes.cpu.exec(1, true);
				break;
			default:
				nes.cpu.exec(1, true);
				break;
		}
	}
}
