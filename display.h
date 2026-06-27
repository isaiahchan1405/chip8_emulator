#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "chip8.h"

#define WINDOW_TITLE  "CHIP-8"
#define CHIP8_WIDTH   64
#define CHIP8_HEIGHT  32
#define SCALE         10   // each CHIP-8 pixel = 10x10 screen pixels
#define PIXEL_ON 0xFFFFFF
#define PIXEL_OFF 0x000000

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} sdl_t;

bool init_sdl(sdl_t *sdl);
void cleanup(sdl_t *sdl);
void clear_screen(sdl_t *sdl);
void render(sdl_t *sdl, chip8_t *chip8); 

#endif