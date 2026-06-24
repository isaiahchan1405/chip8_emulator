#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "display.h"

#define RAM_START 0x200
#define RAM_END 0xFFF

uint8_t chip8_fontset[80] = {     // 16 * 5 bytes
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

typedef struct {
    uint8_t memory[4096];    // RAM, (0x000 to 0x1FF is reserved)
    uint16_t stack[16];      // Call stack
    uint8_t V[16];           // General registers, (V0 to VF)
    uint16_t I;              // Memory address register
    uint16_t PC;             // Program Counter
    uint8_t SP;              // Stack Pointer
    uint8_t DT;              // Dealy timer, counts down at 60Hz (Executes 60 opcodes in one second)
    uint8_t ST;              // Sound timer, counts down at 60Hz, beeps while nonzero
    uint8_t display[64][32]; // Screen buffer
    bool keypad[16];         // Key states (pressed or not)
    uint16_t OP;             // Current Opcode
} chip8_t;

void init_chip8(chip8_t *chip8) {
    // Initialize all bytes to 0s
    memset(chip8, 0, sizeof(chip8_t));

    // Copy fontset
    chip8->PC = RAM_START;
    for (uint8_t i = 0; i < 80; i++) {
        chip8->memory[i] = chip8_fontset[i];
    }
}

// Load file into memory
bool load_rom(chip8_t *chip8, const char *path) {
    FILE *f = fopen(path, "rb");
    if(!f) {
        fprintf(stderr, "Failed to open ROM: %s\n", path);
        return false;
    }

    // Check if ROM fits in memory
    fseek(f, 0, RAM_END);
    long size = ftell(f);
    rewind(f);

    if (size > 4096 - RAM_START) {
        fprintf(stderr, "File too large: %ld bytes", size);
        fclose(f);
        return false;
    }

    // Read file
    fread(&chip8->memory[RAM_START], 1, size, f);
    fclose(f);
    return true
}

void emulate_cycle(chip8_t *chip8) {
    //Fetch Operations
    chip8->OP = chip8->memory[chip8->PC] << 8 | chip8->memory[chip8->PC + 1];
    chip8->PC = chip8->PC + 2;

    //Decode and Execute Opcode Operation
    switch (chip8->OP && 0xF000) {
        case 0x0000:
            switch (chip8->OP & 0x00FF){
                case 0x00E0:  // Clear Display
                    memset(chip8->display, 0, sizeof(chip8->display));
                    break;
                case 0x00EE:  // Return from a subroutine
                    chip8->SP--;
                    chip8->PC = chip8->stack[chip8->SP];
                    break;
                default:     // 0NNN, ignore
                    chip8->PC += 2;
                    break;
            }

        case 0x1000:         // Jump to location nnn (0x1nnn)
            chip8->PC = chip8->OP && 0x0FFF;
            break;

        case 0x2000:         // Call subroutine at nnn
            chip8->SP++;
            chip8->stack[chip8->SP] = chip8->PC;
            chip8->PC = chip8->OP && 0x0FFF;
            break;

        case 0x3000:         // Skip next instruction if Vx == NN
            uint8_t x = (chip8->OP & 0x0F00) >> 8;
            uint8_t NN = (chip8->OP & 0x00FF);
            if(chip8->V[x] == NN)
                chip8->PC += 2;
            break;
        
        case 0x4000:         // Skip next instruction if Vx != NN
            uint8_t x = (chip8->OP & 0x0F00) >> 8;
            uint8_t NN = (chip8->OP & 0x00FF);
            if(chip8->V[x] != NN)
                chip8->PC += 2;
            break;

        case 0x5000:         // Skip next instruction if Vx == Vy
            uint8_t x = (chip8->OP & 0x0F00) >> 8;
            uint8_t y = (chip8->OP & 0x00F0) >> 4;
            if(chip8->V[x] == chip8->V[y])
                chip8->PC += 2;
            break;
        
        case 0x6000:         // Put NN into Vx
            uint8_t x = (chip8->OP & 0x0F00) >> 8;
            uint8_t NN = (chip8->OP & 0x00FF);
            chip8->V[x] = NN;
            break;

        case 0x7000:         // Set Vx = Vx + NN
            uint8_t x = (chip8->OP & 0x0F00) >> 8;
            uint8_t NN = (chip8->OP & 0x00FF);
            chip8->V[x] += NN;
            break;
        
        case 0x8000:         
        switch(chip8->OP && 0xF00F) {
                uint8_t x = (chip8->OP & 0x0F00) >> 8;
                uint8_t y = (chip8->OP & 0x00F0) >> 4;

                case 0x8000: // Set Vx = Vy
                    chip8->V[x] = chip8->V[y];
                    break;
                case 0x8001: // Set Vx = Vx OR Vy
                    chip8->V[x] = chip8->V[x] | chip8->V[y];
                    break;
                case 0x8002: // Set Vx = Vx XOR Vy
                    chip8->V[x] = chip8->V[x] & chip8->V[y];
                    break;
                case 0x8003: // Set Vx = Vx XOR Vy
                    chip8->V[x] = chip8->V[x] ^ chip8->V[y];
                    break;
                case 0x8004: // Set Vx = Vx + Vy, set VF = carry
                    if (chip8->V[x] & chip8->V[y] < chip8->V[x]) 
                        chip8->V[0xF] = 1;
                    else
                        chip8->V[0xF] = 0;
                    chip8->V[x] = chip8->V[x] & chip8->V[y];
                    break;
                case 0x8005: // Set Vx = Vx - Vy, set VF = NOT borrow
                    if(chip8->V[x] >= chip8->V[y])
                        chip8->V[0xF] = 1;
                    else
                        chip8->V[0xF] = 0;
                    chip8->V[x] -= chip8->V[y];
                    break;
                case 0x8006: // Set Vx = Vx SHR(Shift Right) 1
                    chip8->V[0xF] = chip8->V[x] & 0x0F;
                    chip8->V[x] >>= 1;
                    break;
                case 0x8007: // Set Vx = Vy - Vx, set VF = NOT borrow
                    if (chip8->V[y] >= chip8->V[x])
                        chip8->V[0xF] = 1;
                    else
                        chip8->V[0xF] = 0;
                    chip8->V[x] = chip8->V[y] - chip8->V[x];
                case 0x800E: // Set Vx = Vx SHL(Shift Left) 1
                    chip8->V[0xF] = (chip8->V[x] & 0xF0) >> 1;
                    chip8->V[x] <<= 1;
                    break;
            }
        break;

        case 0x9000:         // Skip next instruction if Vx != Vy
            uint8_t x = (chip8->OP & 0x0F00) >> 8;
            uint8_t y = (chip8->OP & 0x00F0) >> 4;
            if (chip8->V[x] != chip8->V[y])
                chip8->PC += 2;
            break;
        
        case 0xA000:         // Set I = nnn
            chip8->I = (chip8->OP & 0x0FFF);
            break;
        
        case 0xB000:         // Set PC = nnn + V0
            chip8->PC = (chip8->OP & 0x0FFF) + chip8->V[0];
            break;
        
        case 0xC000:         // Set Vx = random byte and kk
            srand(time(NULL));
            chip8->V[(chip8->OP & 0xFF00) >> 8] = (rand() % (255 + 1)) & (chip8->OP & 0x00FF);
            break;
        
        case 0xD000:         // Replaces display with sprite; Detects collisions; Wraps sprites if out of bound
            uint8_t x = chip8->V[(chip8->OP & 0x0F00) >> 8];
            uint8_t y = chip8->V[(chip8->OP & 0x00F0) >> 4];
            uint8_t h = chip8->OP & 0x000F;
            
            chip8->V[0xF] = 0; // Clear collision flag

            for (uint8_t row = 0; row<h; row++) {
                uint8_t sprite_byte = chip8->memory[chip8->I + row]; // Fetch one row

                for (uint8_t col = 0; col < 8; col++) {
                    // Verifies if sprite pixel exists (Purely for optimisation)
                    uint8_t sprite_pixel = (sprite_byte >> (7 - col)) & 0x1; 
                    if (!sprite_pixel) continue;

                    // Wrapping
                    uint8_t screen_x = (x + col) % CHIP8_WIDTH;
                    uint8_t screen_y = (y + row) % CHIP8_HEIGHT;
                    
                    // Retrieve and XOR the pixel
                    uint8_t *px = &chip8->display[screen_y][screen_x];
                    if (*px) chip8->V[0xF] = 1;
                    *px ^= 1;
                }
            }

        case 0xE000:
            switch(chip8->OP & 0xF0FF){
                uint8_t x = (chip8->OP & 0x0F00) >> 8;
                case 0xE09E: // Skip next OP if Vx is pressed on keypad
                    if(chip8->keypad[x] == chip8->V[x])
                        chip8->PC += 2;
                case 0xE0A1: // Skip next OP if Vx is not pressed on keypad
                    if(chip8->keypad[x] != chip8->V[x])
                        chip8->PC += 2;
            }
        
        case 0xF000:
            switch(chip8->OP & 0xF0FF){
                uint8_t x = (chip8->OP & 0x0F00) >> 8;
                case 0xF007: // Set Vx = DT value
                    chip8->V[x] = chip8->DT;
                    break;
                case 0xF00A: // Wait for a key press, store key value in Vx
                    for (uint8_t i = 0; i < 0xF; i++) {
                        if (chip8->keypad[i]){
                            chip8->V[x] = i;
                            return;
                        }
                    }
                    // Reverts PC if no key press 
                    chip8->PC -= 2;
                    break;
                case 0xF015: // Set DT = Vx
                    chip8->DT = chip8->V[x];
                    break;
                case 0xF018: // Set ST = Vx
                    chip8->ST = chip8->V[x];
                    break;
                case 0xF01E: // Set I = I + Vx
                    chip8->I = chip8->I + chip8->V[x];
                    break;
                case 0xF029: // Set I = Location of sprite for digit Vx
                    chip8->I = chip8->memory[chip8->V[x] * 5];
                    break;
                case 0xF033: // Store BCD(Binary-Coded Decimal) of Vx in memmory[I, I+1, I+2]
                   // Extract hundred's digit
                    uint8_t huns = chip8->V[x] / 100;
                    chip8->memory[I] = huns;

                   // Extract ten's digit
                    uint8_t tens = chip8->V[x] / 10;
                    chip8->memory[I + 1] = tens;

                   // Extract one's digit
                    uint8_t ones = chip8->V[x] % 10; 
                    chip8->memory[I + 2] = ones;
                    break
                case 0xF055: // Store registers V[0 to x] to memory[I to I + x] (x inclusive)
                    for(int i = 0; 0 <= x; i++) {
                        chip8->memory[chip8->I + i] = chip8->V[i];
                    }
                    break;
                case 0xF065: // Store memory[I to I + x] to registers V[0 to x] (x inclusive) 
                    for(int i = 0; i <= x; i++) {
                        chip8->V[i] = chip8->memory[chip8->I + i];
                    }
            }
    }
}