#include <SDL2/SDL.h>
#include "input.h"
#include "chip8.h"
#include "display.h"
// oh yea i gotta implement audio as well...

#define FPS 60

int main(void)
{
    sdl_t sdl;
    if (!init_sdl(&sdl))
    {
        return 1;
    }

    chip8_t chip8;
    init_chip8(&chip8);

    bool load_successful = load_rom(&chip8, "tests/1-chip8-logo.8o");
    if (!load_successful)
    {
        cleanup(&sdl);
        return 1;
    }

    bool running = true;
    while (running)
    {
        SDL_Event event;
        // Dequeues all events
        while (SDL_PollEvent(&event)) 
            process_input(&chip8, &running, &event);

        for (int i = 0; i < 11; i++)
            emulate_cycle(&chip8);

        render(&sdl, &chip8);
        SDL_Delay(1000 / FPS);
    }

    cleanup(&sdl);
    return 0;
}