#include <iostream>
#include <fstream>

#include <cpu.hpp>

const int MEM_SZ = 0x10000;
const int PRGM_START = 0x300;

int main()
{
	uint8_t ram[MEM_SZ];

	std::ifstream in("prgm", std::ios::binary);

	int i = PRGM_START;
	while (in.read((char *)ram + i++, 1));
	
	ram[i - 1] = 0xff;

	ram[CPU::IRQL] = MSB(PRGM_START);
	ram[CPU::IRQH] = LSB(PRGM_START);

	/*for (int a = 0; a < 10; ++a)
	{
		if (a && !(a % 2))
			printf(" ");
		printf("%02x", ram[PRGM_START + a]);
	}
	printf("\n");
	return 0;*/

	CPU cpu(
		[&](uint16_t addr) -> uint8_t { return ram[addr]; },
		[&](uint16_t addr, uint8_t val) {
			printf("[0x%04x] = 0x%02x\n", addr, val);
			ram[addr] = val;
		}
	);

	cpu.reset();

	std::string s;
	while (true)
	{
		printf("> ");
		std::getline(std::cin, s);

		switch (s[0]) {
			case 'q': exit(0);
			case 'x': {
				uint16_t addr = std::strtol(s.c_str() + 2, nullptr, 16);
				printf("0x%04x: %02x\n", addr, ram[addr]);
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
	}
}
