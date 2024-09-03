CC = gcc
FLAGS = -g
INCLUDES = -I ./include
LIBS = -L ./lib -lSDL2 -lasound -lm
SRC_DIR = ./src
BUILD_DIR = ./build
BIN_DIR = ./bin

SOURCES = memory.c stack.c keyboard.c chip8.c display.c generate_sound.c
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

all: $(BIN_DIR)/main

$(BIN_DIR)/main: $(OBJECTS) $(SRC_DIR)/main.c
	$(CC) $(FLAGS) $(INCLUDES) $(SRC_DIR)/main.c $(OBJECTS) $(LIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(BUILD_DIR)/*.o
	rm -f $(BIN_DIR)/main
