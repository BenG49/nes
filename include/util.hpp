#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <functional>

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

typedef std::function<uint8_t(uint16_t)> bus_read_t;
typedef std::function<void(uint16_t, uint8_t)> bus_write_t;

std::vector<uint8_t> readfile(const char *filename);
std::string to_hex(uint16_t val, uint bytes);
