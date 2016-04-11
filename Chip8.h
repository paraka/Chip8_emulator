#ifndef _CHIP8_H_
#define _CHIP8_H_

#include <stdio.h>
#include <cstdint>
#include <time.h>
#include "Beep.h"
#include "Memory.h"

class Chip8
{
public:
    Chip8(): stack{0}, V{0}, 
             keyboard{0}, display{0}, 
             drawF(true),
             pc(0x200), opcode(0),
             I(0), sp(0),
             delay_timer(0), sound_timer(0) 
             {
                 // init seed for rand()
                 srand(time(NULL));
             } 
    
    ~Chip8() = default;
    Chip8 (const Chip8 &) = delete;
    Chip8 (Chip8 &&) = delete;
    Chip8 & operator=(const Chip8 &) = delete;

    bool LoadROM(const char *filename) { return (memory.loadAppInMemory(filename)); }
    void DumpStatus();
    void RunCicle();

private:
    void DumpMemory();
    void DumpResgisters();
    void DumpStack(); 
    void DumpDisplay();
    void UpdateTimers();

    // Opcode operations stuff
    inline void decodeOpcode();
    inline void clear();
    inline void ret();
    inline void jmp(uint16_t address);
    inline void call(uint16_t address);
    inline void jeq(uint8_t reg, uint8_t value);
    inline void jneq(uint8_t reg, uint8_t value);
    inline void jeqr(uint8_t reg1, uint8_t reg2);
    inline void set(uint8_t reg, uint8_t value);
    inline void add(uint8_t reg, uint8_t value);
    inline void setr(uint8_t reg1, uint8_t reg2);
    inline void Or(uint8_t reg1, uint8_t reg2);
    inline void And(uint8_t reg1, uint8_t reg2);
    inline void Xor(uint8_t reg1, uint8_t reg2);
    inline void addr(uint8_t reg1, uint8_t reg2);
    inline void sub(uint8_t reg1, uint8_t reg2);
    inline void shr(uint8_t reg);
    inline void subb(uint8_t reg1, uint8_t reg2);
    inline void shl(uint8_t reg);
    inline void jneqr(uint8_t reg1, uint8_t reg2);
    inline void seti(uint16_t value);
    inline void jumpv0(uint16_t address);
    inline void Rand(uint8_t reg,uint8_t value);
    inline void draw(uint8_t reg1, uint8_t reg2, uint8_t value);
    inline void jkey(uint8_t reg);
    inline void jnkey(uint8_t reg);
    inline void getdelay(uint8_t reg);
    inline void waitkey(uint8_t reg);
    inline void setdelay(uint8_t reg);
    inline void setsound(uint8_t reg);
    inline void addi(uint8_t reg);
    inline void spritei(uint8_t reg);
    inline void bcd(uint8_t reg);
    inline void push(uint8_t reg);
    inline void pop(uint8_t reg);
    inline void unknown(uint16_t opcode);

public:
    bool drawF;

    // The original implementation of the Chip-8 language used a 64x32-pixel monochrome display: 2048 pixels
    unsigned char display[64 * 32];
    //  The computers which originally used the Chip-8 Language had a 16-key hexadecimal keypad with the following layout:
    // 1   2   3   C
    // 4   5   6   D
    // 7   8   9   E
    // A   0   B   F
    uint8_t keyboard[16];

private:
    // program counter
    uint16_t pc;
    // current opcode
    uint16_t opcode;
    // index register
    uint16_t I;
    // stack pointer
    uint16_t sp;
    
    // Chip-8 has 16 general purpose 8-bit registers, usually referred to as Vx, 
    // where x is a hexadecimal digit (0 through F). 
    // There is also a 16-bit register called I. 
    // This register is generally used to store memory addresses, 
    // so only the lowest (rightmost) 12 bits are usually used.
    uint8_t V[16]; 

    // The stack is an array of 16 16-bit values, used to store the address 
    // that the interpreter shoud return to when finished with a subroutine. 
    // Chip-8 allows for up to 16 levels of nested subroutines.
    uint16_t stack[16]; 

    //The Chip-8 language is capable of accessing up to 4KB (4,096 bytes) of RAM, 
    // from location 0x000 (0) to 0xFFF (4095). 
    // The first 512 bytes, from 0x000 to 0x1FF, are where the original 
    // interpreter was located, and should not be used by programs.
    Memory<4096> memory;

    //Chip-8 provides 2 timers, a delay timer and a sound timer.
    uint8_t delay_timer;
    uint8_t sound_timer;

    //To make beep
    Beep beeper;
};

#endif // _CHIP8_H_
