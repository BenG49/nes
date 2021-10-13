#pragma once

#include <cpu.hpp>

#include <vector>

void run_tests(CPU *cpu);

void cpu_test(std::vector<uint8_t> binfile, std::function<bool(CPU*)> verify, CPU *cpu);
