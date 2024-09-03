#include "memory.h"
#include "config.h"
#include <assert.h>

static void is_memory_in_bounds(int address) {
  assert(address >= 0 && address < MEMORY_SIZE);
}

unsigned char memory_read(struct Memory *memory, int address) {
  is_memory_in_bounds(address);
  return memory->memory[address];
}

unsigned short memory_read_short(struct Memory *memory, int address) {
  is_memory_in_bounds(address);
  return (memory->memory[address] << 8) | memory->memory[address + 1];
}

void memory_write(struct Memory *memory, int address, unsigned char value) {
  is_memory_in_bounds(address);
  memory->memory[address] = value;
}

// void memory_load_character_set(struct Memory *memory) {
//   unsigned char character_set[CHARACTER_SET_SIZE] = CHARACTER_SET;
//   for (int i = 0; i < CHARACTER_SET_SIZE; i++) {
//     memory->memory[CHARACTER_SET_START_ADDRESS + i] = character_set[i];
//   }
// }
  
