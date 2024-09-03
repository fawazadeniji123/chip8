#ifndef CHIP_H
#define CHIP_H

#include "config.h"
#include "display.h"
#include "keyboard.h"
#include "memory.h"
#include "registers.h"
#include "stack.h"
#include <stddef.h>

struct Chip8 {
  struct Memory memory;
  struct Registers registers;
  struct Stack stack;
  struct Keyboard keyboard;
  struct Display display;
};

void chip8_init(struct Chip8 *chip8);
void chip8_load_program(struct Chip8 *chip8, const unsigned char *program,
                        size_t size);
void chip8_exec(struct Chip8 *chip8, unsigned short opcode);
void chip8_cycle(struct Chip8 *chip8);

#endif