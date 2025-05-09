#include <iostream>
#include <random>
#include <string>

#include <nes.hpp>

// something weird with instruction at C1EB, needs to read 0 from $40
int main(int argc, const char *argv[])
{
	NES nes(readfile("pacman.nes"));

	// nes.run();
	nes.run(&CPU::trace);

	nes.cpu.halted = true;
}
