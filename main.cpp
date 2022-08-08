#include <iostream>
#include <random>
#include <string>

#include <nes.hpp>

int main(int argc, const char *argv[])
{
	NES nes(readfile("pacman.nes"));

	// nes.run();
	nes.run(&CPU::trace);

	nes.cpu.halted = true;
}
