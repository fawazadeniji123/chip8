#include "keyboard.h"
#include <assert.h>
#include <stdbool.h>

static void is_key_in_bounds(int key) { assert(key >= 0 && key < KEY_COUNT); }

void keyboard_init(struct Keyboard *keyboard, const char *map) {
  keyboard->key_map = map;
}

int keyboard_map_key(struct Keyboard *keyboard, char key) {
  for (int i = 0; i < KEY_COUNT; i++) {
    if (keyboard->key_map[i] == key) {
      return i;
    }
  }

  return -1;
}

void keyboard_press(struct Keyboard *keyboard, int key) {
  is_key_in_bounds(key);
  keyboard->keys[key] = true;
}

void keyboard_release(struct Keyboard *keyboard, int key) {
  is_key_in_bounds(key);
  keyboard->keys[key] = false;
}

bool keyboard_is_pressed(struct Keyboard *keyboard, int key) {
  is_key_in_bounds(key);
  return keyboard->keys[key];
}