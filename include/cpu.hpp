#pragma once

#include <cinttypes>
#include <iostream>
#include <fstream>

#include <util.hpp>

typedef uint8_t cpu_t;
typedef uint16_t addr_t;

const int MEM_SZ = 0x10000;
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
	
	uint8_t reg() const
	{
		return (neg << 7) | (ov << 6) | 0x20 | (brk << 4) | (dec << 3) | (intr << 2) | (zero << 1) | carry;
	}

	void set_int(uint8_t reg)
	{
		neg = 	GET_BIT(reg, 7);
		ov = 	GET_BIT(reg, 6);
		dec = 	GET_BIT(reg, 3);
		intr = 	GET_BIT(reg, 2);
		zero = 	GET_BIT(reg, 1);
		carry =	GET_BIT(reg, 0);
	}
	
	void set(uint8_t reg)
	{
		neg = 	GET_BIT(reg, 7);
		ov = 	GET_BIT(reg, 6);
		brk = 	GET_BIT(reg, 4);
		dec = 	GET_BIT(reg, 3);
		intr = 	GET_BIT(reg, 2);
		zero = 	GET_BIT(reg, 1);
		carry =	GET_BIT(reg, 0);
	}
	
	void print() const
	{
		printf("N=%d V=%d B=%d D=%d I=%d Z=%d C=%d\n", neg, ov, brk, dec, intr, zero, carry);
	}
};

struct CPU {
	// NOTE: stack pointer = 0x100 + sp
	// stack grows down
	cpu_t a, x, y, sp;

	Flags f;

	uint8_t mem[MEM_SZ];
	addr_t ip;

	CPU() : a(), x(), y(), sp(0xff), mem(), ip(PRGM_START) {}
	CPU(std::ifstream &in) : CPU()
	{
		int i;

		for (i = PRGM_START; !in.eof(); ++i)
			in >> mem[i];

		// HCF
		mem[i] = 0xff;
	}
	
	// sets flags other than overflow, decimal
	void set_flags(uint16_t val, Flags to_set);
};
