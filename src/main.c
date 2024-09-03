#include "chip8.h"
#include "generate_sound.h"
#include "keyboard.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

const char key_map[KEY_COUNT] = {
    SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_q, SDLK_w, SDLK_e, SDLK_r,
    SDLK_a, SDLK_s, SDLK_d, SDLK_f, SDLK_z, SDLK_x, SDLK_c, SDLK_v,
};

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <program>\n", argv[0]);
    return 1;
  }

  const char *program_path = argv[1];
  printf("Loading program: %s\n", program_path);

  FILE *file = fopen(program_path, "rb");
  if (!file) {
    printf("Error: Could not open file %s\n", program_path);
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  unsigned char *program = (unsigned char *)malloc(file_size);
  if (!program) {
    printf("Error: Could not allocate memory for program\n");
    return 1;
  }

  size_t result = fread(program, 1, file_size, file);
  if (result != file_size) {
    printf("Error: Could not read file %s\n", program_path);
    return 1;
  }
  printf("Program size: %ld bytes\n", file_size);

  fclose(file);

  struct Chip8 chip8;
  chip8_init(&chip8);
  display_init(&chip8.display);

  // load the program into memory
  chip8_load_program(&chip8, program, file_size);
  printf("Program loaded successfully\n");

  // initialize the keyboard
  keyboard_init(&chip8.keyboard, key_map);

  // free the program memory
  free(program);

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return 1;
  }


  while (1) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        goto out;
        break;
      case SDL_KEYDOWN: {
        char key = event.key.keysym.sym;
        int virtual_key = keyboard_map_key(&chip8.keyboard, key);
        if (virtual_key != -1) {
          keyboard_press(&chip8.keyboard, virtual_key);
        }
      } break;
      case SDL_KEYUP: {
        char key = event.key.keysym.sym;
        int virtual_key = keyboard_map_key(&chip8.keyboard, key);
        if (virtual_key != -1) {
          keyboard_release(&chip8.keyboard, virtual_key);
        }
      } break;
      }
    }

    // implement a delay using delay_timer register
    if (chip8.registers.delay_timer > 0) {
      SDL_Delay(.1);
      chip8.registers.delay_timer--;
    }

    // implement sound
    if (chip8.registers.sound_timer > 0) {
      beep(440, 2 * chip8.registers.sound_timer, 0.1f);
      chip8.registers.sound_timer = 0;
    }

    chip8_cycle(&chip8);
    display_draw(&chip8.display);
  }

out:
  SDL_DestroyWindow(chip8.display.window);
  SDL_Quit();

  return 0;
}
