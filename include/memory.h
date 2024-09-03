#ifndef MEMORY_H
#define MEMORY_H

#include "config.h"

struct Memory {
  unsigned char memory[MEMORY_SIZE];
};

unsigned char memory_read(struct Memory *memory, int address);
unsigned short memory_read_short(struct Memory *memory, int address);
void memory_write(struct Memory *memory, int address, unsigned char value);
void memory_load_fontset(struct Memory *memory);

#endif