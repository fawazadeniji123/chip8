#include "display.h"
#include "chip8.h"
#include <SDL2/SDL.h>
#include <assert.h>
#include <stdbool.h>

static void check_display_bounds(int x, int y) {
  assert(x >= 0 && x < DISPLAY_WIDTH);
  assert(y >= 0 && y < DISPLAY_HEIGHT);
}

void display_init(struct Display *display) {
  display->window =
      SDL_CreateWindow(EMULAOR_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH * PIXEL_SIZE,
                       DISPLAY_HEIGHT * PIXEL_SIZE, SDL_WINDOW_SHOWN);
  if (display->window == NULL) {
    printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  display->renderer =
      SDL_CreateRenderer(display->window, -1, SDL_TEXTUREACCESS_TARGET);
  if (display->renderer == NULL) {
    printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  display->texture = SDL_CreateTexture(
      display->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
      DISPLAY_WIDTH * PIXEL_SIZE, DISPLAY_HEIGHT * PIXEL_SIZE);
  if (display->texture == NULL) {
    printf("SDL_CreateTexture Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  display_clear(display);
}

void display_draw(struct Display *display) {
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 0);
  SDL_RenderClear(display->renderer);
  SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 0);

  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      if (display_get_pixel(display, x, y)) {
        SDL_Rect rect = {x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE,
                         PIXEL_SIZE};
        SDL_RenderFillRect(display->renderer, &rect);
      }
    }
  }

  SDL_RenderPresent(display->renderer);
}

bool display_draw_sprite(struct Display *display, int x, int y,
                         const unsigned char *sprite, int n) {
  bool collision = false;

  for (int ly = 0; ly < n; ly++) {
    unsigned char byte = sprite[ly];
    for (int lx = 0; lx < 8; lx++) {
      // check if the pixel is set
      if (byte & (0b10000000 >> lx)) {
        int pixel_x = (x + lx) % DISPLAY_WIDTH;
        int pixel_y = (y + ly) % DISPLAY_HEIGHT;
        if (display_get_pixel(display, pixel_x, pixel_y)) {
          collision = true;
        }
        display_set_pixel(display, pixel_x, pixel_y,
                          display_get_pixel(display, pixel_x, pixel_y) ^ 1);
      }
    }
  }

  // display->draw_flag = true;

  return collision;
}

void display_set_pixel(struct Display *display, int x, int y, bool value) {
  check_display_bounds(x, y);
  display->pixels[y][x] = value;
}

bool display_get_pixel(struct Display *display, int x, int y) {
  check_display_bounds(x, y);
  return display->pixels[y][x];
}

void display_clear(struct Display *display) {
  memset(display->pixels, 0, sizeof(display->pixels));
}
