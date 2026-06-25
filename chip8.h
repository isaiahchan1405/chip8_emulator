#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
#include <stdint.h>

#define RAM_START 0x200
#define RAM_END 0xFFF
#define CHIP8_FONTSET_SIZE 80

typedef struct {
	uint8_t memory[4096];
	uint16_t stack[16];
	uint8_t V[16];
	uint16_t I;
	uint16_t PC;
	uint8_t SP;
	uint8_t DT;
	uint8_t ST;
	bool display[CHIP8_HEIGHT][CHIP8_WIDTH];
	bool keypad[16];
	uint16_t OP;
} chip8_t;

extern const uint8_t chip8_fontset[CHIP8_FONTSET_SIZE];

void init_chip8(chip8_t *chip8);
bool load_rom(chip8_t *chip8, const char *path);
void emulate_cycle(chip8_t *chip8);

#endif