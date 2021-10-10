#pragma once

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define GET_BIT(var, pos) (((var) >> (pos)) & 1)
#define MSB(var) (var >> 8)
#define LSB(var) (var & 0xff)
