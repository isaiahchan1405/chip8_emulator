#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

bool init_sdl(sdl_t *sdl)
void cleanup(sdl_t *sdl)
void clear_screen(sdl_t *sdl)
void render(sdl_t *sdl, const bool display[CHIP8_HEIGHT][CHIP8_WIDTH]) 