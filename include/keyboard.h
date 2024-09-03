#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include "config.h"

struct Chip8;

struct Keyboard {
  bool keys[KEY_COUNT];
  const char *key_map;
};

void keyboard_init(struct Keyboard *keyboard, const char *map);
int keyboard_map_key(struct Keyboard *keyboard, char key);
void keyboard_press(struct Keyboard *keyboard, int key);
void keyboard_release(struct Keyboard *keyboard, int key);
bool keyboard_is_pressed(struct Keyboard *keyboard, int key);

#endif