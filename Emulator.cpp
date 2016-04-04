#include <SDL2/SDL.h>
#include <unistd.h>

#include "Chip8.h"
#include "Graphics.h"

#ifdef DEBUG
#include "Debug.h"
#endif

class Emulator
{
public:
    Emulator() : graphics(processor) {}

    ~Emulator() = default;

    bool LoadROM(const char *filename) { return (processor.LoadROM(filename)); }

    void Dump()
    {
        processor.DumpStatus();
    }

    void Run()
    {
        graphics.mainLoop();
    }

#ifdef DEBUG
    void Debug()
    {
        int key = 0;
        do
        {
            key = getche();
            Dump();
            if (key == 0x20) processor.RunCicle();
        } while (key == 0x20);
    }
#endif

private:
    Chip8 processor;
    Graphics graphics;
};

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Usage: %s <ROM file>\n\n", argv[0]);
        return 1;
    }

    Emulator emu;

    if(!emu.LoadROM(argv[1]))       
        return 1;

#ifdef DEBUG
    emu.Dump();
#endif

    printf("Starting emulation...\n");

#ifdef DEBUG
    emu.Debug();
#else
    emu.Run();
#endif

    return 0;
}
