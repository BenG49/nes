#pragma once

#include <cpu.hpp>

bool cpu_test(std::ifstream &test, int instrs, std::function<bool(CPU&)> verify);
