#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#define WINDOW_TITLE  "CHIP-8"
#define CHIP8_WIDTH   64
#define CHIP8_HEIGHT  32
#define SCALE         10   // each CHIP-8 pixel = 10x10 screen pixels

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
} sdl_t;

bool init_sdl(sdl_t *sdl) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow (
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        CHIP8_WIDTH*SCALE,
        CHIP8_HEIGHT*SCALE,
        0
    );

    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, 0);

    if(!sdl->window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }
    
    if(!sdl->renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
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

// Draw the CHIP-8 display buffer to the screen.
// display is a 64x32 array of bools — true = pixel on, false = off.
void render(sdl_t *sdl, const bool display[CHIP8_HEIGHT][CHIP8_WIDTH]) {
    clear_screen(sdl);

    SDL_SetRenderDrawColor(sdl->renderer, 255, 255, 255, 255);  // white pixels
    for (int y = 0; y < CHIP8_HEIGHT; y++) {
        for (int x = 0; x < CHIP8_WIDTH; x++) {
            if (display[y][x]) {
                SDL_Rect pixel = {
                    .x = x * SCALE,
                    .y = y * SCALE,
                    .w = SCALE,
                    .h = SCALE,
                };
                SDL_RenderFillRect(sdl->renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(sdl->renderer);
}

/*
int main(void) {
    sdl_t sdl = {0};
    if (!init_sdl(&sdl)) return 1;

    // Stub display buffer — all pixels off
    // replace this with your real CHIP-8 display memory later
    bool display[CHIP8_HEIGHT][CHIP8_WIDTH] = {0};

    // Light up a few pixels to confirm rendering works
    display[0][0] = true;
    display[10][11] = true;
    display[11][10] = true;
    display[11][11] = true;

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        render(&sdl, display);
        SDL_Delay(16);  // ~60fps
    }

    cleanup(&sdl);
    return 0;
}
*/