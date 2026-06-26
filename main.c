#include <SDL2/SDL.h>
#include "input.h"
#include "chip8.h"
#include "display.h"
// oh yea i gotta implement audio as well...

#define FPS 60

int main(void) {
    sdl_t sdl;
    init_sdl(&sdl);

    chip8_t chip8;
    init_chip8(&chip8);

    bool load_successful = load_rom(&chip8, "path");
    if (!load_successful) return 0;

    bool running = true;
    while (running) {
        SDL_Event event;
        // Dequeues all events
        while(SDL_PollEvent(&event)) {
            process_input(&chip8, &running, &event);
        }

        emulate_cycle(&chip8);

        render(&sdl, &chip8);

        SDL_delay(1000 / FPS);
    }

    return 0;
}
/*
initialise sdl and chip8
while running
    deque OS input
    Read memory, process Opcode
    render
    pause
*/