#pragma once

#include <vector>
#include <string>
#include <cstdint>

#define GET_BIT(var, pos) (((var) >> (pos)) & 1)
#define SET_BIT(var, flag, pos) { \
	if (flag) {                   \
		(var) |= (1 << (pos));    \
	} else {                      \
		(var) &= (~(1 << (pos))); \
	}                             \
	}

#define MSB(var) ((var) >> 8)
#define LSB(var) ((var) & 0xFF)

#define JMP_BUS(addr) pc = (bus_read(addr + 1) << 8) | bus_read(addr);

std::vector<uint8_t> readfile(const char *filename);
std::string to_hex(uint16_t val, uint bytes);
