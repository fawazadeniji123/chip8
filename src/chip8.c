#include "chip8.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char default_character_set[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 0xF0, 0x10,
    0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 0x90, 0x90, 0xF0, 0x10,
    0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0,
    0x10, 0x20, 0x40, 0x40, 0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0,
    0x10, 0xF0, 0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 0xF0, 0x80,
    0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80,
};

void chip8_init(struct Chip8 *chip8) {
  // Initialize memory
  memset(chip8, 0, sizeof(struct Chip8));

  // Load the default character set into memory
  memcpy(&chip8->memory.memory, default_character_set,
         sizeof(default_character_set));

  // Set the program counter to the start of the program
  chip8->registers.PC = PROGRAM_START_ADDRESS;

  // Set the stack pointer to zero
  chip8->registers.SP = 0;

  // Set the delay and sound timers to zero
  chip8->registers.delay_timer = 0;
  chip8->registers.sound_timer = 0;

  // Set the keyboard keys to zero
  memset(&chip8->keyboard.keys, 0, sizeof(chip8->keyboard.keys));
}

void chip8_load_program(struct Chip8 *chip8, const unsigned char *program,
                        size_t size) {
  // Check if the program will fit in memory
  if (size > MEMORY_SIZE - PROGRAM_START_ADDRESS) {
    fprintf(stderr, "Program is too large to fit in memory\n");
    return;
  }

  // Copy the program into memory
  memcpy(&chip8->memory.memory[PROGRAM_START_ADDRESS], program, size);

  // Set the program counter to the start of the program
  chip8->registers.PC = PROGRAM_START_ADDRESS;

  // Set the stack pointer to zero
  chip8->registers.SP = 0;

  // Set the delay and sound timers to zero
  chip8->registers.delay_timer = 0;
  chip8->registers.sound_timer = 0;

  // Set the keyboard keys to zero
  memset(&chip8->keyboard.keys, 0, sizeof(chip8->keyboard.keys));
}

static char chip8_wait_for_keypress(struct Chip8 *chip8) {
  SDL_Event event;
  while (SDL_WaitEvent(&event)) {
    if (event.type == SDL_KEYDOWN) {
      char key = event.key.keysym.sym;
      char virtual_key = keyboard_map_key(&chip8->keyboard, key);
      if (virtual_key != -1) {
        return virtual_key;
      }
    }
  }

  return -1;
}

static void exec_0NNN(struct Chip8 *chip8, unsigned short opcode) {
  switch (opcode & 0x00FF) {
  case 0xE0:
    // Clear the display
    display_clear(&chip8->display);
    break;
  case 0xEE:
    // Return from a subroutine
    chip8->registers.PC = stack_pop(chip8);
    break;
  default:
    // Call RCA 1802 program at address NNN
    break;
  }
}

static void exec_8XYN(struct Chip8 *chip8, unsigned short opcode) {
  unsigned char N = opcode & 0x000F;
  unsigned char X = (opcode & 0x0F00) >> 8;
  unsigned char Y = (opcode & 0x00F0) >> 4;

  switch (N) {
  case 0x0:
    // Set V[X] to V[Y]
    chip8->registers.V[X] = chip8->registers.V[Y];
    break;
  case 0x1:
    // Set V[X] to V[X] OR V[Y]
    chip8->registers.V[X] |= chip8->registers.V[Y];
    break;
  case 0x2:
    // Set V[X] to V[X] AND V[Y]
    chip8->registers.V[X] &= chip8->registers.V[Y];
    break;
  case 0x3:
    // Set V[X] to V[X] XOR V[Y]
    chip8->registers.V[X] ^= chip8->registers.V[Y];
    break;
  case 0x4:
    // Add V[Y] to V[X] and set V[F] to 1 if there is a carry
    chip8->registers.V[0xF] =
        (chip8->registers.V[X] + chip8->registers.V[Y]) > 0xFF;
    chip8->registers.V[X] += chip8->registers.V[Y];
    // keep the lowest 8 bits
    chip8->registers.V[X] &= 0xFF;
    break;
  case 0x5:
    // Subtract V[Y] from V[X] and set V[F] to 0 if there is a borrow
    chip8->registers.V[0xF] = chip8->registers.V[X] > chip8->registers.V[Y];
    chip8->registers.V[X] -= chip8->registers.V[Y];
    break;
  case 0x6:
    // Shift V[X] right by 1 and set V[F] to the least significant bit of V[X]
    chip8->registers.V[0xF] = chip8->registers.V[X] & 0x1;
    chip8->registers.V[X] >>= 1;
    break;
  case 0x7:
    // Set V[X] to V[Y] - V[X] and set V[F] to 0 if there is a borrow
    chip8->registers.V[0xF] = chip8->registers.V[Y] > chip8->registers.V[X];
    chip8->registers.V[X] = chip8->registers.V[Y] - chip8->registers.V[X];
    break;
  case 0xE:
    // Shift V[X] left by 1 and set V[F] to the most significant bit of V[X]
    chip8->registers.V[0xF] = chip8->registers.V[X] & 0x80;
    chip8->registers.V[X] <<= 1;
    break;
  default:
    break;
  }
}

static void exec_DXYN(struct Chip8 *chip8, unsigned short opcode) {
  // Draw a sprite at (V[X], V[Y]) with width 8 and height N
  // Each row of 8 pixels is read as bit-coded starting from memory location I
  // I value does not change after the execution of this instruction
  // V[F] is set to 1 if any screen pixels are flipped from set to unset when
  // the sprite is drawn and to 0 if that does not happen
  unsigned char N = opcode & 0x000F;
  unsigned char X = (opcode & 0x0F00) >> 8;
  unsigned char Y = (opcode & 0x00F0) >> 4;

  const unsigned char *sprite =
      (const unsigned char *)&chip8->memory.memory[chip8->registers.I];
  unsigned char VX = chip8->registers.V[X];
  unsigned char VY = chip8->registers.V[Y];

  bool collision = display_draw_sprite(&chip8->display, VX, VY, sprite, N);

  chip8->registers.V[0xF] = collision;
}

static void exec_EXNN(struct Chip8 *chip8, unsigned short opcode) {
  unsigned char X = (opcode & 0x0F00) >> 8;
  unsigned char NN = opcode & 0x00FF;

  switch (NN) {
  case 0x9E:
    // Skip next instruction if key with the value of V[X] is pressed
    if (keyboard_is_pressed(&chip8->keyboard, chip8->registers.V[X])) {
      chip8->registers.PC += 2;
    }
    break;
  case 0xA1:
    // Skip next instruction if key with the value of V[X] is not pressed
    if (!keyboard_is_pressed(&chip8->keyboard, chip8->registers.V[X])) {
      chip8->registers.PC += 2;
    }
    break;
  default:
    break;
  }
}

static void exec_FXNN(struct Chip8 *chip8, unsigned short opcode) {
  unsigned char X = (opcode & 0x0F00) >> 8;
  unsigned char NN = opcode & 0x00FF;

  switch (NN) {
  case 0x07:
    // Set V[X] to the value of the delay timer
    chip8->registers.V[X] = chip8->registers.delay_timer;
    break;
  case 0x0A:
    // Wait for a key press and store the value of the key in V[X]
    chip8->registers.V[X] = chip8_wait_for_keypress(chip8);
    break;
  case 0x15:
    // Set the delay timer to V[X]
    chip8->registers.delay_timer = chip8->registers.V[X];
    break;
  case 0x18:
    // Set the sound timer to V[X]
    chip8->registers.sound_timer = chip8->registers.V[X];
    break;
  case 0x1E:
    // Add V[X] to I
    chip8->registers.I += chip8->registers.V[X];
    break;
  case 0x29:
    // Set I to the location of the sprite for the character in V[X]
    chip8->registers.I = CHARACTER_SET_START_ADDRESS +
                         chip8->registers.V[X] * CHARACTER_SET_HEIGHT;
    break;
  case 0x33:
    // Store the binary-coded decimal representation of V[X] at I, I + 1, and I
    // + 2
    memory_write(&chip8->memory, chip8->registers.I,
                 chip8->registers.V[X] / 100);
    memory_write(&chip8->memory, chip8->registers.I + 1,
                 (chip8->registers.V[X] / 10) % 10);
    memory_write(&chip8->memory, chip8->registers.I + 2,
                 chip8->registers.V[X] % 10);
    break;
  case 0x55:
    // Store V[0] to V[X] in memory starting at address I
    for (int i = 0; i <= X; i++) {
      memory_write(&chip8->memory, chip8->registers.I + i,
                   chip8->registers.V[i]);
    }
    break;
  case 0x65:
    // Fill V[0] to V[X] with values from memory starting at address I
    for (int i = 0; i <= X; i++) {
      chip8->registers.V[i] =
          memory_read(&chip8->memory, chip8->registers.I + i);
    }
    break;
  default:
    break;
  }
}

void chip8_exec(struct Chip8 *chip8, unsigned short opcode) {
  unsigned short NNN = opcode & 0x0FFF;
  unsigned char KK = opcode & 0x00FF;
  unsigned char N = opcode & 0x000F;
  unsigned char X = (opcode & 0x0F00) >> 8;
  unsigned char Y = (opcode & 0x00F0) >> 4;

  switch (opcode & 0xF000) {
  case 0x0000:
    exec_0NNN(chip8, opcode);
    break;
  case 0x1000:
    // Jump to address NNN
    chip8->registers.PC = NNN;
    break;
  case 0x2000:
    // Call subroutine at NNN
    stack_push(chip8, chip8->registers.PC);
    chip8->registers.PC = NNN;
    break;
  case 0x3000:
    // Skip next instruction if V[X] == KK
    if (chip8->registers.V[X] == KK) {
      chip8->registers.PC += 2;
    }
    break;
  case 0x4000:
    // Skip next instruction if V[X] != KK
    if (chip8->registers.V[X] != KK) {
      chip8->registers.PC += 2;
    }
    break;
  case 0x5000:
    // Skip next instruction if V[X] == V[Y]
    if (chip8->registers.V[X] == chip8->registers.V[Y]) {
      chip8->registers.PC += 2;
    }
    break;
  case 0x6000:
    // Set V[X] to KK
    chip8->registers.V[X] = KK;
    break;
  case 0x7000:
    // Add KK to V[X]
    chip8->registers.V[X] += KK;
    break;
  case 0x8000:
    exec_8XYN(chip8, opcode);
    break;
  case 0x9000:
    // Skip next instruction if V[X] != V[Y]
    if (chip8->registers.V[X] != chip8->registers.V[Y]) {
      chip8->registers.PC += 2;
    }
    break;
  case 0xA000:
    // Set I to NNN
    chip8->registers.I = NNN;
    break;
  case 0xB000:
    // Jump to address NNN + V[0]
    chip8->registers.PC = NNN + chip8->registers.V[0];
    break;
  case 0xC000:
    srand(clock());
    // Set V[X] to a random number AND KK
    chip8->registers.V[X] = (rand() % 255) & KK;
    break;
  case 0xD000:
    exec_DXYN(chip8, opcode);
    break;
  case 0xE000:
    exec_EXNN(chip8, opcode);
    break;
  case 0xF000:
    exec_FXNN(chip8, opcode);
    break;
  default:
    break;
  }
}

void chip8_cycle(struct Chip8 *chip8) {
  // Fetch the opcode
  unsigned short opcode =
      memory_read_short(&chip8->memory, chip8->registers.PC);

  // Increment the program counter
  chip8->registers.PC += 2;

  // Execute the opcode
  chip8_exec(chip8, opcode);
}
