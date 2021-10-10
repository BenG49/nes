#include <iostream>

#include <interpret.hpp>

const char *file = "lolol";

int main()
{
	CPU cpu;
	Interpret i(&cpu);

	while (1)
		i.interpret();
}
