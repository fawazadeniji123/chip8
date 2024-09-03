#include "stack.h"
#include "chip8.h"
#include <assert.h>

static void is_stack_in_bounds(int SP) { assert(SP >= 0 && SP < STACK_SIZE); }

void stack_push(struct Chip8 *chip8, unsigned short value) {
  is_stack_in_bounds(chip8->registers.SP + 1);
  chip8->stack.stack[++chip8->registers.SP] = value;
}

unsigned short stack_pop(struct Chip8 *chip8) {
  is_stack_in_bounds(chip8->registers.SP);
  return chip8->stack.stack[chip8->registers.SP--];
}