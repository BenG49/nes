CUR_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

CC=g++
CFLAGS=-Wall -Wno-switch -I$(CUR_DIR)/include -std=c++17 -g

SRCS=$(shell find $(CUR_DIR)/src/ -type f -name '*.cpp')
HDRS=$(shell find $(CUR_DIR)/include/ -type f -name '*.hpp')
OBJS=${SRCS:.cpp=.o}

TARGET=a.out

.PHONY: run clean debug

run: $(TARGET)
	$(CUR_DIR)/$(TARGET)

$(TARGET): $(OBJS) $(HDRS) $(CUR_DIR)/main.cpp
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(CUR_DIR)/main.cpp

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)

debug: $(TARGET)
	gdb --args $(TARGET)
