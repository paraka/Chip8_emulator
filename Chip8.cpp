#include <cstdlib>
#include "Chip8.h"

void Chip8::DumpStatus()
{
    DumpMemory();
    DumpResgisters();
    DumpStack();
    DumpDisplay();
}

void Chip8::DumpMemory() 
{ 
    printf("\nMEMORY STATUS: \n");
    memory.Dump(); 
}

void Chip8::DumpResgisters()
{
    printf("\nREGISTERS STATUS: \n");
    printf("V0  V1  V2  V3  V4  V5  V6  V7  V8  V9  VA  VB  VC  VD  VE  VF\n");
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

void Chip8::DumpDisplay()
{
    printf("\nDISPLAY STATUS: \n");
    for(auto i=0;i<2048;++i)
    {
        printf("%X ", display[i]);
        if ((i + 1) % 64 == 0) { printf("\n"); }
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

void Chip8::clear()
{
    memset(display, 0, 2048);
    drawF = true;
}

void Chip8::ret()
{
    //We have to restore stuff from stack
    //so we put into program counter stored return address
    --sp;
    pc = stack[sp];
}

void Chip8::jmp(uint16_t address)
{
    pc = address;
}

void Chip8::call(uint16_t address)
{
    // save return address(pc) into the stack
    stack[sp] = pc;
    ++sp;
    pc = address;
}

void Chip8::jeq(uint8_t reg, uint8_t value)
{
    V[reg] == value ? pc += 4 : pc += 2;
}

void Chip8::jneq(uint8_t reg, uint8_t value)
{
    V[reg] != value ? pc += 4 : pc += 2;
}

void Chip8::jeqr(uint8_t reg1, uint8_t reg2)
{
    V[reg1] == V[reg2] ? pc += 4 : pc += 2;
}

void Chip8::set(uint8_t reg, uint8_t value)
{
    V[reg] = value;
}

void Chip8::add(uint8_t reg, uint8_t value)
{
    V[reg] += value;
}

void Chip8::setr(uint8_t reg1, uint8_t reg2)
{
    V[reg1] = V[reg2];
}

void Chip8::Or(uint8_t reg1, uint8_t reg2)
{
    V[reg1] |= V[reg2];
}

void Chip8::And(uint8_t reg1, uint8_t reg2)
{
    V[reg1] &= V[reg2];
}

void Chip8::Xor(uint8_t reg1, uint8_t reg2)
{
    V[reg1] ^= V[reg2];
}

void Chip8::addr(uint8_t reg1, uint8_t reg2)
{
    V[0xF] = (0xFF - V[reg1]) < V[reg2];
    V[reg1] += V[reg2];
}

void Chip8::sub(uint8_t reg1, uint8_t reg2)
{
    V[0xF] = V[reg1] < V[reg2]; 
    V[reg1] -= V[reg2];
}

void Chip8::shr(uint8_t reg)
{
    V[0xF] = V[reg] & 0x1;
    V[reg] >>= 1;
}

void Chip8::subb(uint8_t reg1, uint8_t reg2)
{
    V[0xF] = V[reg1] > V[reg2];
    V[reg1] = V[reg2] - V[reg1];
}

void Chip8::shl(uint8_t reg)
{
    V[0xF] = V[reg] >> 7;
    V[reg] <<= 1;
}

void Chip8::jneqr(uint8_t reg1, uint8_t reg2)
{
    V[reg1] != V[reg2] ? pc += 4 : pc += 2;
}

void Chip8::seti(uint16_t value)
{
    I = value;
}

void Chip8::jumpv0(uint16_t address)
{
    pc = address + V[0];
}

void Chip8::Rand(uint8_t reg,uint8_t value)
{
    V[reg] = value & (rand() % 0xFF); 
}

void Chip8::draw(uint8_t reg1, uint8_t reg2, uint8_t value)
{
    uint8_t x = V[reg1];
    uint8_t y = V[reg2];
    uint8_t rows = value;
    V[0xF] = 0;
    for (auto j=0; j<rows; j++)
    {
        uint8_t sprite = memory.Read(I + j);
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
}

void Chip8::jkey(uint8_t reg)
{
    keyboard[V[reg]] != 0 ? pc += 4 : pc += 2;
}

void Chip8::jnkey(uint8_t reg)
{
    keyboard[V[reg]] == 0 ? pc += 4 : pc += 2;
}

void Chip8::getdelay(uint8_t reg)
{
    V[reg] = delay_timer;
}

void Chip8::waitkey(uint8_t reg)
{
    bool pressed = false;
    for (auto i=0; i<16; ++i)
    {
        if (keyboard[i] != 0)
        {
            V[reg] = i;
            pressed = true;
        }
    }
    if (!pressed) return;
    pc += 2;
}

void Chip8::setdelay(uint8_t reg)
{
    delay_timer = V[reg];
}

void Chip8::setsound(uint8_t reg)
{
    sound_timer = V[reg];
}

void Chip8::addi(uint8_t reg)
{
    V[0xF] = (I + V[reg]) > 0xFFF;
    I += V[reg];
}

void Chip8::spritei(uint8_t reg)
{
    I = V[reg] * 0x5;
}

void Chip8::bcd(uint8_t reg)
{
    memory.Write(I, V[reg] / 100);
    memory.Write(I + 1, (V[reg] / 10) % 10);
    memory.Write(I + 2, V[reg] % 10);
}

void Chip8::push(uint8_t reg)
{
    for (auto i=0; i<= reg; ++i) memory.Write(I+i, V[i]);
    I += reg + 1;
}

void Chip8::pop(uint8_t reg)
{
    for (auto i=0; i<= reg; ++i) V[i] = memory.Read(I+i);
    I += reg + 1;
}

void Chip8::unknown(uint16_t opcode)
{
    printf ("Unknown opcode: 0x%X\n", opcode);
}

void Chip8::decodeOpcode()
{
    // fetch opcode: Opcode is 2bytes long -> we have to fetch two bytes
    // Ej: 0xA2       0xA2 << 8 = 0xA200 
    // Use or bitwase two join it: 0xA200 | 0xF0 = 0xA2F0
    opcode = memory[pc] << 8 | memory[pc + 1];
}

// https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#2.5 -> Standard chip8 instructions
void Chip8::RunCicle()
{
    decodeOpcode();

    // process opcode
    // Check MSB of first byte 
    switch(opcode & 0xF000)
    {
        case 0x0000:
            // Check LSB of second byte
            switch(opcode & 0x000F)
            {
                case 0x0000: // 0x00E0: Clears the screen 
                    clear();
                    pc += 2; 
                    break;
                case 0x000E: // 0x00EE: Returns from subrutine
                    ret();
                    pc += 2; 
                    break;
                default:
                    unknown(opcode);
            }
            break;
        case 0x1000: //  0x1NNN: Jumps to address NNN.
            jmp(opcode & 0x0FFF);
            break;
        case 0x2000: // 0x2NNN: Calls subroutine at NNN.
            call(opcode & 0x0FFF);
            break;
        case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN.
            jeq((opcode & 0x0F00) >> 8, opcode & 0x00FF);
            break;
        case 0x4000: // 0x4XNN: Skips the next instruction if VX doesn't equal NN. 
            jneq((opcode & 0x0F00) >> 8, opcode & 0x00FF);
            break;
        case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY.
            jeqr((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0x6000: // 0x6XNN: Sets VX to NN.
            set((opcode & 0x0F00) >> 8, opcode & 0x00FF);
            pc += 2;
            break;
        case 0x7000: // 0x7XNN: Adds NN to VX.
            add((opcode & 0x0F00) >> 8, opcode & 0x00FF);
            pc += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F)
            {
                case 0x0000: // 0x8XY0: Sets VX to the value of VY.
                    setr((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
                    pc += 2;
                    break;
                case 0x0001: // 0x8XY1: Sets VX to VX or VY.
                    Or((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
                    pc += 2;
                    break;
                case 0x0002: // 0x8XY2: Sets VX to VX and VY
                    And((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
                    pc += 2;
                    break;
                case 0x0003: // 0x8XY3: Sets VX to VX xor VY
                    Xor((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
                    pc += 2;
                    break;
                case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                    addr((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
                    pc += 2;
                    break;
                case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    sub((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
                    pc += 2;
                    break;
                case 0x0006: // 0x8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
                    shr((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                case 0x0007: // 0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
                    subb((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
                    pc += 2;
                    break;
                case 0x000E: // 0x8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
                    shl((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                default:
                    unknown(opcode);
            }
            break;
        case 0x9000: // 9XY0: Skips the next instruction if VX doesn't equal VY.
            jneqr((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
            break;
        case 0xA000: // 0xANNN: Sets I to the address NNN.
            seti(opcode & 0x0FFF);
            pc += 2;
            break;
        case 0xB000: // 0xBNNN: Jumps to the address NNN plus V0.
            jumpv0(opcode & 0x0FFF);
            break;
        case 0xC000: // 0xCXNN: Sets VX to the result of a bitwise and operation on a random number and NN.
            Rand((opcode & 0x0F00) >> 8, opcode & 0x00FF);
            pc += 2;
            break;
        case 0xD000: // 0xDXYN: Sprites stored in memory at location in index register (I), 8bits wide. 
                     // Wraps around the screen. If when drawn, clears a pixel, 
                     // register VF is set to 1 otherwise it is zero. 
                     // All drawing is XOR drawing (i.e. it toggles the screen pixels). 
                     // Sprites are drawn starting at position VX, VY. N is the number 
                     // of 8bit rows that need to be drawn. 
                     // If N is greater than 1, second line continues at position VX, VY+1, and so on.
            draw((opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4,  opcode & 0x000F);
            pc += 2;
            break;
        case 0xE000: 
            switch(opcode & 0x00FF)
            {
                case 0x009E: // 0xEX9E: Skips the next instruction if the key stored in VX is pressed. 
                    jkey((opcode & 0x0F00) >> 8);
                    break;
                case 0x00A1: // 0xEXA1: Skips the next instruction if the key stored in VX isn't pressed.
                    jnkey((opcode & 0x0F00) >> 8);
                    break;
                default:
                    unknown(opcode);
                    
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x0007: // 0xFX07: Sets VX to the value of the delay timer.
                    getdelay((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                case 0x000A: // 0xFX0A: A key press is awaited, and then stored in VX.
                    waitkey((opcode & 0x0F00) >> 8);
                    break;
                case 0x0015: // 0xFX15: Sets the delay timer to VX. 
                    setdelay((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                case 0x0018: // 0xFX18: Sets the sound timer to VX.
                    setsound((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                case 0x001E: // 0xFX1E: Adds VX to I.
                    // VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't. 
                    // This is undocumented feature of the CHIP-8 and used by Spacefight 2091! game.
                    addi((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                case 0x0029: // 0xFX29: Sets I to the location of the sprite for the character in VX. 
                             // Characters 0-F (in hexadecimal) are represented by a 4x5 font.
                    spritei((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                case 0x0033: // 0xFX33: Stores the Binary-coded decimal representation of VX, with the most significant 
                             // of three digits at the address in I, the middle digit at I plus 1, 
                             // and the least significant digit at I plus 2. 
                             // (In other words, take the decimal representation of VX, 
                             // place the hundreds digit in memory at location in I, 
                             // the tens digit at location I+1, and the ones digit at location I+2.)
                    bcd((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                case 0x0055: // 0xFX55: Stores V0 to VX (including VX) in memory starting at address I.
                             // On the original interpreter, when the operation is done, I=I+X+1. On current implementations, I is left unchanged.
                    push((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                case 0x0065: // 0xFX65: Fills V0 to VX (including VX) with values from memory starting at address I.
                             // On the original interpreter, when the operation is done, I=I+X+1. On current implementations, I is left unchanged.
                    pop((opcode & 0x0F00) >> 8);
                    pc += 2;
                    break;
                default:
                    unknown(opcode);
            }
            break;
        default:
            unknown(opcode);
    }

    UpdateTimers();
}
