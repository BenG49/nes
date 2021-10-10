#pragma once

#include <cinttypes>
#include <iostream>

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define GET_BIT(var, pos) (((var) >> (pos)) & 1)

typedef uint8_t cpu_t;
typedef uint16_t addr_t;

const int MEM_SZ = 0x1000;
const int PRGM_START = 0x300;

// NV-BDIZC
struct Flags {
	bool neg 	: 1;
	bool ov 	: 1;

	bool brk 	: 1;
	bool dec 	: 1;
	bool intr 	: 1;
	bool zero 	: 1;
	bool carry 	: 1;

	Flags() : neg(), ov(), brk(), dec(), intr(), zero(), carry() {}
	Flags(bool neg, bool zero, bool carry)
		: neg(neg), ov(false), brk(false), dec(false), intr(false), zero(zero), carry(carry) {}
	
	void print() const
	{
		printf("N(%d) V(%d) B(%d) D(%d) I(%d) Z(%d) C(%d)\n", neg, ov, brk, dec, intr, zero, carry);
	}
};

struct CPU {
	// NOTE: stack pointer = 0x100 + sp
	// stack grows down
	cpu_t a, x, y, sp;

	Flags f;

	uint8_t mem[MEM_SZ];
	addr_t ip;

	CPU() : a(), x(), y(), sp() , mem() , ip(PRGM_START) {}
	
	// sets flags other than overflow, decimal
	void set_flags(uint16_t val, Flags to_set);
};
