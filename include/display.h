#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "config.h"

struct Display {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  bool pixels[DISPLAY_HEIGHT][DISPLAY_WIDTH];
  bool draw_flag;
};

void display_init(struct Display *display);
void display_draw(struct Display *display);
bool display_draw_sprite(struct Display *display, int x, int y, const unsigned char *sprite, int n);
void display_set_pixel(struct Display *display, int x, int y, bool value);
bool display_get_pixel(struct Display *display, int x, int y);
void display_clear(struct Display *display);

#endif