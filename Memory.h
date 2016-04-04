#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdio.h>
#include <memory>

/* Memory Map:
+---------------+= 0xFFF (4095) End of Chip-8 RAM
|               |
|               |
|               |
|               |
|               |
| 0x200 to 0xFFF|
|     Chip-8    |
| Program / Data|
|     Space     |
|               |
|               |
|               |
+- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
|               |
|               |
|               |
+---------------+= 0x200 (512) Start of most Chip-8 programs
| 0x000 to 0x1FF|
| Reserved for  |
|  interpreter  |
+---------------+= 0x000 (0) Start of Chip-8 RAM
*/

namespace
{
    // 16 x 5 
    // http://devernay.free.fr/hacks/chip8/C8TECH10.HTM -> DISPLAY
    unsigned char chip8_fontset[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };
}

template <uint32_t B>
class Memory
{
public:
    Memory() : memory{0} 
    {
        LoadFontSet();
    }
   
    void Write(int address, uint16_t value)
    {
        memory[address] = value;
    }

    uint16_t Read(int address)
    {
        return(memory[address]);
    }

    bool loadAppInMemory(const char * filename)
    {
        printf("Loading: %s\n", filename);

        // Open file
        FILE * pFile = fopen(filename, "rb");
        if (!pFile) return false;

        fseek(pFile , 0 , SEEK_END);
        long lSize = ftell(pFile);
        rewind(pFile);
        printf("Filesize: %d\n", (int)lSize);

        using BufferPtr = std::unique_ptr<char>;
        BufferPtr buffer(new char[sizeof(char) * lSize]);
        if (!buffer) return false;

        size_t result = fread (buffer.get(), 1, lSize, pFile);
        if (result != lSize) return false;

        bool ret = true;
        if ((B-BASE_START) > lSize)
        {
            for(int i = 0; i < lSize; ++i)
                memory[i + BASE_START] = buffer.get()[i];
        }
        else
        {
            printf("Error: ROM too big for memory");
            ret = false;
        }

        fclose(pFile);

        return ret;
    }

    void DumpFontSet()
    {
        for (auto i=0; i<RESERVED; ++i)
        {
            printf("%2X ", memory[i]);
            if ((i + 1) % 5 == 0) { printf("\n"); }
        }
    }

    void Dump()
    {
        for (auto i=BASE_START; i<B; ++i)
        {
            printf("%2X ", memory[i]);
            if ((i + 1) % 16 == 0) { printf("\n"); }
        }
    }

    uint16_t operator[](std::size_t idx)       { return memory[idx]; };
    const uint16_t operator[](std::size_t idx) const { return memory[idx]; };

private:

    void LoadFontSet()
    {
        for (auto i=0; i<RESERVED; ++i)
        {
            Write(i, chip8_fontset[i]);
        }
    }
                            
private:
    static const int RESERVED = 80;
    static const int BASE_START = 512; // 0x200
    unsigned char memory[B]; 
};

#endif // _MEMORY_H_
