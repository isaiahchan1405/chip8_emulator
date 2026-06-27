#ifndef INPUT_H
#define INPUT_H

#include "chip8.h"
#include <SDL2/SDL.h>

void process_input(chip8_t *chip8, bool *running, SDL_Event *event);

#endif