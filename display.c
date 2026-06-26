#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "display.h"
#include "chip8.h"

bool init_sdl(sdl_t *sdl) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    // Initialise SDL window
    sdl->window = SDL_CreateWindow (
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        CHIP8_WIDTH*SCALE,
        CHIP8_HEIGHT*SCALE,
        0
    );
    if(!sdl->window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    // Initialise SDL Renderer
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, 0);  
    if(!sdl->renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return false;
    }

    // Initialise SDL Texture
    sdl->texture = SDL_CreateTexture (
    sdl->renderer,
    SDL_PIXELFORMAT_RGBA8888,
    SDL_TEXTUREACCESS_STREAMING, 
    CHIP8_WIDTH, CHIP8_HEIGHT
    );
    if(!sdl->texture) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        return false;
    }

    return true;
}


void cleanup(sdl_t *sdl) {
    SDL_DestroyRenderer(sdl->renderer);
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();
}

void clear_screen(sdl_t *sdl) {
    SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 255);   // black
    SDL_RenderClear(sdl->renderer);
}

void render(sdl_t *sdl, chip8_t *chip8) {
    // Read display values to color values in screen_buffer
    uint32_t screen_buffer[CHIP8_HEIGHT * CHIP8_WIDTH];
    for(int i; i < CHIP8_HEIGHT * CHIP8_WIDTH; i++) {
        screen_buffer[i] = chip8->display[i] ? PIXEL_ON : PIXEL_OFF;
    }

    // Convert screen_buffer to texture
    SDL_UpdateTexture(sdl->texture, NULL, screen_buffer, CHIP8_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(sdl->renderer);
    SDL_RenderCopy(sdl->renderer, sdl->texture, NULL, NULL); 
    SDL_RenderPresent(sdl->renderer);
}
