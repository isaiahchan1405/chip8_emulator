#include "chip8.h"
#include <SDL2/SDL.h>

/*
CHIP-8 Keypad   SDL Keyboard
   1 2 3 C        1 2 3 4
   4 5 6 D        Q W E R
   7 8 9 E        A S D F
   A 0 B F        Z X C V
*/

void process_input(chip8_t *chip8, bool *running, SDL_Event *event) {
    switch (event->type) {
        case SDL_QUIT:
            *running = false;
            break;
        
        // Map SDL keyboard press to CHIP-8 keypad press
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE: *running = false; break;
                case SDLK_1: chip8->keypad[0x1] = 1; break;
                case SDLK_2: chip8->keypad[0x2] = 1; break;
                case SDLK_3: chip8->keypad[0x3] = 1; break;
                case SDLK_4: chip8->keypad[0xC] = 1; break;
                case SDLK_q: chip8->keypad[0x4] = 1; break;
                case SDLK_w: chip8->keypad[0x5] = 1; break;
                case SDLK_e: chip8->keypad[0x6] = 1; break;
                case SDLK_r: chip8->keypad[0xD] = 1; break;
                case SDLK_a: chip8->keypad[0x7] = 1; break;
                case SDLK_s: chip8->keypad[0x8] = 1; break;
                case SDLK_d: chip8->keypad[0x9] = 1; break;
                case SDLK_f: chip8->keypad[0xE] = 1; break;
                case SDLK_z: chip8->keypad[0xA] = 1; break;
                case SDLK_x: chip8->keypad[0x0] = 1; break;
                case SDLK_c: chip8->keypad[0xB] = 1; break;
                case SDLK_v: chip8->keypad[0xF] = 1; break;
            }
            break;
        
        // Map SDL keyboard release to CHIP-8 keypad release
        case SDL_KEYUP:
            switch (event->key.keysym.sym) {
                case SDLK_1: chip8->keypad[0x1] = 0; break;
                case SDLK_2: chip8->keypad[0x2] = 0; break;
                case SDLK_3: chip8->keypad[0x3] = 0; break;
                case SDLK_4: chip8->keypad[0xC] = 0; break;
                case SDLK_q: chip8->keypad[0x4] = 0; break;
                case SDLK_w: chip8->keypad[0x5] = 0; break;
                case SDLK_e: chip8->keypad[0x6] = 0; break;
                case SDLK_r: chip8->keypad[0xD] = 0; break;
                case SDLK_a: chip8->keypad[0x7] = 0; break;
                case SDLK_s: chip8->keypad[0x8] = 0; break;
                case SDLK_d: chip8->keypad[0x9] = 0; break;
                case SDLK_f: chip8->keypad[0xE] = 0; break;
                case SDLK_z: chip8->keypad[0xA] = 0; break;
                case SDLK_x: chip8->keypad[0x0] = 0; break;
                case SDLK_c: chip8->keypad[0xB] = 0; break;
                case SDLK_v: chip8->keypad[0xF] = 0; break;
            }
            break;            
    }
}