#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

bool init_sdl(void){
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0){
        SDL_Log("Could not initialize SDL subsystems %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void final_cleanup(void){
    SDL_Quit();
}

int main(int argc, char **argv){
    (void)argc;
    (void)argv;

    //Initialize SDL
    //

    //Final cleanup
    //

    puts("testing on linux");
}