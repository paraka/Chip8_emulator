#include "Chip8.h"

namespace
{
    // Display resolution is 64×32 pixels, and color is monochrome.
    static const uint32_t SCREEN_WIDTH = 64;
    static const uint32_t SCREEN_HEIGHT = 32;
    static constexpr uint32_t display_width = SCREEN_WIDTH * 10;
    static constexpr uint32_t display_height = SCREEN_HEIGHT * 10;
}

class Emulator
{
public:
    Emulator() = default;
    ~Emulator() = default;

    bool LoadROM(const char *filename) { return (processor.LoadROM(filename)); }

    void DumpMemory()
    {
        processor.DumpMemory();
    }

private:
    Chip8 processor;
};

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s <ROM fike>\n\n", argv[0]);
        return 1;
    }

    Emulator emu;

    if(!emu.LoadROM(argv[1]))       
        return 1;

    emu.DumpMemory();
    printf("Starting emulation...\n");

    return 0;
}
