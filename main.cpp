#include <SDL2/SDL.h>

#include <iostream>
#include <random>
#include <string>

#include <nes.hpp>

// TODO: add masswerk virtual 6502 trace style
int main(int argc, const char *argv[])
{
	NES nes(readfile("nestest.nes"));

	nes.cpu.exec_with_callback(&CPU::trace);
}
