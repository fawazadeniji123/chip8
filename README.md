# Chip 8 Interpreter/Emulator

This is a simple Chip 8 interpreter/emulator written in c. It is not the most efficient implementation, but it is simple and easy to understand. The interpreter is capable of running most Chip 8 programs, but there are still some bugs that need to be fixed.

## Chip 8

Chip 8 is an interpreted programming language that was designed by Joseph Weisbecker in the mid 1970s for the COSMAC VIP computer. It was later used on the Telmac 1800 and some other computers. Chip 8 programs are run on a virtual machine that has 4k of memory, a 64x32 pixel monochrome display, and a 16 key keypad. The Chip 8 has 16 general purpose 8-bit registers, a 16-bit index register, a 16-bit program counter, and a 16-bit stack pointer. The Chip 8 has 16 levels of stack, a delay timer, and a sound timer. The delay timer is decremented at a rate of 60Hz, and the sound timer is decremented at a rate of 60Hz. When the sound timer is non-zero, a tone is played.

## Programs

There are many Chip 8 programs available on the internet. Some of the most popular programs include Pong, Space Invaders, and Tetris. The interpreter is capable of running most Chip 8 programs, but there are still some bugs that need to be fixed.

<!-- ## Screenshots

![Pong](screenshots/pong.png)
![Space Invaders](screenshots/space_invaders.png)
![Tetris](screenshots/tetris.png) -->

## Features

- [x] 640x320 pixel monochrome display
- [x] Sound
- [x] Timers
- [ ] Super Chip 8 support
- [ ] Save and load state
- [ ] Debugger
- [ ] Disassembler
- [ ] Assembler
- [ ] Documentation
- [ ] Tests
- [ ] Code cleanup

## Prerequisites

To build the interpreter, you will need the following:

- A C compiler (gcc, clang, etc.)
- GNU Make
- SDL2

### Ubuntu

To install the required libraries on Ubuntu, run the following command:

```bash
sudo apt-get install build-essential libsdl2-dev 
```

### Fedora

To install the required libraries on Fedora, run the following command:

```bash
sudo dnf install SDL2-devel
```

### Arch Linux

To install the required libraries on Arch Linux, run the following command:

```bash
sudo pacman -S sdl2 
```

### macOS

To install the required libraries on macOS, run the following command:

```bash
brew install sdl2 
```

## Building

To build the interpreter, simply run the following command:

```bash
make
```

## Running

To run a Chip 8 program, simply run the following command:

```bash
./chip8 <path_to_rom>
```

## Controls

The Chip 8 has a 16 key keypad:

```text
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```

The Chip 8 keypad is mapped to the following keys on the keyboard:

```text
1 2 3 4
Q W E R
A S D F
Z X C V
```

## References

- [Cowgod's Chip 8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8)
- [How to write an emulator (CHIP-8 interpreter)](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
- [Mastering Chip 8](http://mattmik.com/files/chip8/mastering/chip8.html)
- [CHIP-8 Emulator](http://www.codeslinger.co.uk/pages/projects/chip8.html)
- [CHIP-8 Emulator](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
- [CHIP-8 Emulator](http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
