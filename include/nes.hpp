#pragma once

#include <cpu.hpp>

#include <fstream>

struct NES {
	CPU cpu;

	NES(std::ifstream &ines);
};
