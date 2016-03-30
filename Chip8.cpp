#include "Chip8.h"

void Chip8::UpdateTimers()
{
    if(delay_timer > 0) --delay_timer;

    if(sound_timer > 0)
    {
        if(sound_timer == 1) printf("BEEP!\n");
        --sound_timer;
    }
}

// https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.5 -> Standard chip8 instructions
void Chip8::RunCicle()
{
    // fetch opcode: Opcode is 2bytes long -> we have to fetch two bytes
    // Ej: 0xA2       0xA2 << 8 = 0xA200 
    // Use or bitwase two join it: 0xA200 | 0xF0 = 0xA2F0
    opcode = memory[pc] << 8 | memory[pc + 1];

    // process opcode
    // Check MSB of first byte 
    switch(opcode & 0xF000)
    {
        case 0x0000:
            break;
        case 0x1000:
            break;
        case 0x2000:
            break;
        case 0x3000:
            break;
        case 0x4000:
            break;
        case 0x5000:
            break;
        case 0x6000:
            break;
        case 0x7000:
            break;
        case 0x8000:
            break;
        case 0x9000:
            break;
        case 0xA000:
            break;
        case 0xB000:
            break;
        case 0xC000:
            break;
        case 0xD000:
            break;
        case 0xE000:
            break;
        case 0xF000:
            break;
        default:
            printf ("Unknown opcode: 0x%X\n", opcode);
    }

    UpdateTimers();
}
