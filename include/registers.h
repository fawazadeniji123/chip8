#ifndef REGISTERS_H
#define REGISTERS_H

#include "config.h"

struct Registers {
  unsigned char V[REGISTER_COUNT];
  unsigned short I;
  unsigned char delay_timer;
  unsigned char sound_timer;
  unsigned short PC;
  unsigned char SP;
};

#endif