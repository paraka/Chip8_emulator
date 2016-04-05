#include <cstdlib>
#include "Chip8.h"

void Chip8::DumpStatus()
{
    DumpMemory();
    DumpResgisters();
    DumpStack();
}

void Chip8::DumpMemory() 
{ 
    printf("\nMEMORY STATUS: \n");
    memory.DumpFontSet(); 
    memory.Dump(); 
}

void Chip8::DumpResgisters()
{
    printf("\nREGISTERS STATUS: \n");
    printf("V1  V2  V3  V4  V5  V6  V7  V8  V9  VA  VB  VC  VD  VF\n");
    for(auto i=0;i<16;++i)
    {
        printf("%2X  ", V[i]);
    }
    printf("\n");
    printf("I: %4X\n", I);
    printf("PC: %4X\n", pc);
    printf("SP: %4X\n", sp);
}

void Chip8::DumpStack()
{
    printf("\nSTACK STATUS: \n");
    for(auto i=0;i<16;++i)
    {
        printf("%2X ", stack[i]);
        if ((i + 1) % 4 == 0) { printf("\n"); }
    }
}

void Chip8::UpdateTimers()
{
    if(delay_timer > 0) --delay_timer;

    if(sound_timer > 0)
    {
        if (--sound_timer == 0) beeper.StopBeep();
        else beeper.StartBeep();
    }
}

void Chip8::ClearScreen()
{
    for(int i = 0; i < 2048; ++i)
        display[i] = 0x0;
    drawF = true;
}

void Chip8::RestorePCFromStack()
{
    //We have to restore stuff from stack
    //so we put into program counter stored return address
    --sp;
    pc = stack[sp];
}

void Chip8::SavePCIntoTheStack()
{
    // save return address(pc) into the stack
    stack[sp] = pc;
    ++sp;
}

void Chip8::JumpToAddress(uint16_t address)
{
    pc = address;
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
            // Check LSB of second byte
            switch(opcode & 0x000F)
            {
                case 0x0000: // 0x00E0: Clears the screen 
                    ClearScreen();
                    pc += 2; 
                    break;
                case 0x000E: // 0x00EE: Returns from subrutine
                    RestorePCFromStack();
                    pc += 2; 
                    break;
                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);
            }
            break;
        case 0x1000: //  0x1NNN: Jumps to address NNN.
            JumpToAddress(opcode & 0x0FFF);
            break;
        case 0x2000: // 0x2NNN: Calls subroutine at NNN.
            SavePCIntoTheStack();
            JumpToAddress(opcode & 0x0FFF);
            break;
        case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN.
            V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF) ? pc += 4 : pc += 2;
            break;
        case 0x4000: // 0x4XNN: Skips the next instruction if VX doesn't equal NN. 
            V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF) ? pc += 4 : pc += 2;
            break;
        case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY.
            V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4] ? pc += 4 : pc += 2;
            break;
        case 0x6000: // 0x6XNN: Sets VX to NN.
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;
        case 0x7000: // 0x7XNN: Adds NN to VX.
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000: // 0x8XY0: Sets VX to the value of VY.
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0001: // 0x8XY1: Sets VX to VX or VY.
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0002: // 0x8XY2: Sets VX to VX and VY
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0003: // 0x8XY3: Sets VX to VX xor VY
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                    V[0xF] = (0xFF - V[(opcode & 0x0F00) >> 8]) < V[(opcode & 0x00F0) >> 4];
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    V[0xF] = V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4]; 
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;
                case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    V[0xF] = V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4];
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;
                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);
            }
            break;
        case 0x9000: // 9XY0: Skips the next instruction if VX doesn't equal VY.
            V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4] ? pc += 4 : pc += 2;
            break;
        case 0xA000: // 0xANNN: Sets I to the address NNN.
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000: // 0xBNNN: Jumps to the address NNN plus V0.
            JumpToAddress((opcode & 0x0FFF) + V[0]);
            break;
        case 0xC000: // 0xCXNN: Sets VX to the result of a bitwise and operation on a random number and NN.
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) & (rand() % 0xFF); 
            pc += 2;
            break;
        case 0xD000: // 0xDXYN: Sprites stored in memory at location in index register (I), 8bits wide. 
                     // Wraps around the screen. If when drawn, clears a pixel, 
                     // register VF is set to 1 otherwise it is zero. 
                     // All drawing is XOR drawing (i.e. it toggles the screen pixels). 
                     // Sprites are drawn starting at position VX, VY. N is the number 
                     // of 8bit rows that need to be drawn. 
                     // If N is greater than 1, second line continues at position VX, VY+1, and so on.
        {
            uint16_t x = V[(opcode & 0x0F00) >> 8];
            uint16_t y = V[(opcode & 0x00F0) >> 4];
            uint16_t rows = opcode & 0x000F;
            uint16_t pixel = 0;
            V[0xF] = 0;
            for (auto j=0; j<rows; j++)
            {
                uint16_t sprite = memory.Read(I + j);
                for (auto i=0; i<8; i++)
                {
                    int px = (x + i) & 63;
                    int py = (y + j) & 31;
                    int pos = 64 * py + px;
                    int pixel = (sprite & (1 << (7-i))) != 0;
                    V[0xF] |= (display[pos] & pixel);
                    display[pos] ^= pixel;
                }
            }
            drawF = true;
            pc += 2;
        }
            break;
        case 0xE000: 
            switch(opcode & 0x00FF)
            {
                case 0x009E: // 0xEX9E: Skips the next instruction if the key stored in VX is pressed. 
                    keyboard[V[(opcode & 0x0F00) >> 8]] != 0 ? pc += 4 : pc += 2;
                    break;
                case 0x00A1: // 0xEXA1: Skips the next instruction if the key stored in VX isn't pressed.
                    keyboard[V[(opcode & 0x0F00) >> 8]] == 0 ? pc += 4 : pc += 2;
                    break;
                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007: // 0xFX07: Sets VX to the value of the delay timer.
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;
                case 0x000A: // 0xFX0A: A key press is awaited, and then stored in VX.
                {
                    bool pressed = false;
                    for (auto i=0; i<16; ++i)
                    {
                        if (keyboard[i] != 0)
                        {
                            V[(opcode & 0x0F00) >> 8] = keyboard[i];
                            pressed = true;
                        }
                    }
                    if (!pressed) return;
                    pc += 2;
                }
                    break;
                case 0x0015: // 0xFX15: Sets the delay timer to VX. 
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x0018: // 0xFX18: Sets the sound timer to VX.
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x001E: // 0xFX1E: Adds VX to I.
                    // VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't. 
                    // This is undocumented feature of the CHIP-8 and used by Spacefight 2091! game.
                    V[0xF] = (I + V[(opcode & 0x0F00) >> 8]) > 0xFFF;
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x0029: // 0xFX29: Sets I to the location of the sprite for the character in VX. 
                             // Characters 0-F (in hexadecimal) are represented by a 4x5 font.
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;
                case 0x0033: // 0xFX33: Stores the Binary-coded decimal representation of VX, with the most significant 
                             // of three digits at the address in I, the middle digit at I plus 1, 
                             // and the least significant digit at I plus 2. 
                             // (In other words, take the decimal representation of VX, 
                             // place the hundreds digit in memory at location in I, 
                             // the tens digit at location I+1, and the ones digit at location I+2.)
                    memory.Write(I, V[(opcode & 0x0F00) >> 8] / 100);
                    memory.Write(I + 1, (V[(opcode & 0x0F00) >> 8] / 10) % 10);
                    memory.Write(I + 2, V[(opcode & 0x0F00) >> 8] % 10);
                    pc += 2;
                    break;
                case 0x0055: // 0xFX55: Stores V0 to VX (including VX) in memory starting at address I.
                             // On the original interpreter, when the operation is done, I=I+X+1. On current implementations, I is left unchanged.
                    for (auto i=0; i<= V[(opcode & 0x0F00) >> 8]; ++i) memory.Write(I+i, V[i]);
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;
                case 0x0065: // 0xFX65: Fills V0 to VX (including VX) with values from memory starting at address I.
                             // On the original interpreter, when the operation is done, I=I+X+1. On current implementations, I is left unchanged.
                    for (auto i=0; i<= V[(opcode & 0x0F00) >> 8]; ++i) V[i] = memory.Read(I+i);
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;
                default:
                    printf ("Unknown opcode: 0x%X\n", opcode);
            }
            break;
        default:
            printf ("Unknown opcode: 0x%X\n", opcode);
    }

    UpdateTimers();
}
