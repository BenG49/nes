CUR_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

CC=g++
CFLAGS=-Wall -Wno-switch -I$(CUR_DIR)/include -std=c++17 -g

SRCS=$(shell find $(CUR_DIR)/src/ -type f -name '*.cpp')
HDRS=$(shell find $(CUR_DIR)/include/ -type f -name '*.hpp')
OBJS=${SRCS:.cpp=.o}
PRGM=prgm.asm

TARGET=a.out
PRGM_TARGET=prgm

.PHONY: run clean debug prgm

$(TARGET): $(OBJS) $(HDRS) $(CUR_DIR)/main.cpp
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(CUR_DIR)/main.cpp

prgm: $(PRGM)
	vasm6502_oldstyle -Fbin -dotdir $< -o $(PRGM_TARGET)

run: $(TARGET) $(PRGM_TARGET)
	$(CUR_DIR)/$(TARGET)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)

debug: $(TARGET)
	gdb --args $(TARGET)
