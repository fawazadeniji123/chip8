#ifndef STACK_H
#define STACK_H

#include "config.h"

struct Chip8;

struct Stack {
  unsigned short stack[STACK_SIZE];
};

void stack_push(struct Chip8 *chip8, unsigned short value);
unsigned short stack_pop(struct Chip8 *chip8);

#endif