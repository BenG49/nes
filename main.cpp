#include <iostream>

#include <nes.hpp>
#include <test.hpp>

int main(int argc, const char *argv[])
{
	std::ifstream in(argv[1], std::ios::binary);

	cpu_test(in,  )

	// NES nes(in);

	/*std::string s;
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
