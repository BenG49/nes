#include <iostream>

#include <interpret.hpp>

int main()
{
	std::ifstream in("prgm", std::ios::binary);
	CPU cpu(in);
	Emul i(&cpu);

	while (true)
		i.interpret();
}
