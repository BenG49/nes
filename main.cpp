#include <SDL2/SDL.h>

#include <iostream>
#include <random>
#include <string>

#include <nes.hpp>

int main(int argc, const char *argv[])
{
	NES nes(readfile("nestest.nes"));

	nes.run(&CPU::trace);
}
