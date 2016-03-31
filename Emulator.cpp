#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <unistd.h>

#include "Chip8.h"

#ifdef DEBUG
#include "Debug.h"
#endif

namespace
{
    // Display resolution is 64×32 pixels, and color is monochrome.
    static const uint32_t SCREEN_WIDTH = 64;
    static const uint32_t SCREEN_HEIGHT = 32;
    //static constexpr uint32_t display_width = SCREEN_WIDTH * 10;
    //static constexpr uint32_t display_height = SCREEN_HEIGHT * 10;
    static uint32_t display_width = SCREEN_WIDTH * 10;
    static uint32_t display_height = SCREEN_HEIGHT * 10;
}

class Graphics
{
public:
    explicit Graphics(Chip8 &chip8) 
            : window(NULL),
              renderer(NULL),
              texture(NULL), 
              chip8(chip8)
    {
        Init();
    }

    ~Graphics() = default;

    void Init()
    {
        SDL_Init(SDL_INIT_EVERYTHING);
        window = SDL_CreateWindow("CHIP-8 Emulator",
                                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     display_height, display_width, SDL_WINDOW_SHOWN);
        if (window == NULL) 
        {
            SDL_Quit();
            exit(1);
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (renderer == NULL) 
        {
            SDL_DestroyWindow(window);
            SDL_Quit();
            exit(1);
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
        if (texture == NULL) 
        {
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();
            exit(1);
        }
    }

    void Updatekey(SDL_KeyboardEvent* e, uint32_t val)
    {
        switch (e->keysym.sym)
        {
            case SDLK_1:
                chip8.keyboard[0x1] = val;
                break;
            case SDLK_2:
                chip8.keyboard[0x2] = val;
                break;
            case SDLK_3:
                chip8.keyboard[0x3] = val;
                break;
            case SDLK_4:
                chip8.keyboard[0xC] = val;
                break;

            case SDLK_q:
                chip8.keyboard[0x4] = val;
                break;
            case SDLK_w:
                chip8.keyboard[0x5] = val;
                break;
            case SDLK_e:
                chip8.keyboard[0x6] = val;
                break;
            case SDLK_r:
                chip8.keyboard[0xD] = val;
                break;

            case SDLK_a:
                chip8.keyboard[0x7] = val;
                break;
            case SDLK_s:
                chip8.keyboard[0x8] = val;
                break;
            case SDLK_d:
                chip8.keyboard[0x9] = val;
                break;
            case SDLK_f:
                chip8.keyboard[0xE] = val;
                break;

            case SDLK_z:
                chip8.keyboard[0xA] = val;
                break;
            case SDLK_x:
                chip8.keyboard[0x0] = val;
                break;
            case SDLK_c:
                chip8.keyboard[0xB] = val;
                break;
            case SDLK_v:
                chip8.keyboard[0xF] = val;
                break;
        }
    }

    void reshape(SDL_Window *w, int width, int height) 
    {
        glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glViewport(0, 0, width, height);

        display_width = width;
        display_height = height;
    }

    void resize(SDL_WindowEvent* e)
    {
        if (e->windowID == SDL_GetWindowID(window)) 
        {
            SDL_SetWindowSize(window, e->data1, e->data2);
            reshape(window, e->data1, e->data2);
        }
    }

    void expand_screen(unsigned char* from, uint32_t * to)
    {
        for (int i = 0; i < 2048; i++)
            to[i] = (from[i]) ? -1 : 0;
    }

    void display()
    {
        chip8.RunCicle();

        if (chip8.drawF)
        {
            void*   pixels;
            int     pitch;

            /* Update SDL Texture with current data in CPU. */
            SDL_LockTexture(texture, NULL, &pixels, &pitch);
            expand_screen(chip8.display, (uint32_t *) pixels);
            SDL_UnlockTexture(texture);

            /* Render the texture. */
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            chip8.drawF = false;
        }
    }

    void mainLoop()
    {
        bool running = true;

        while(running)
        {
            SDL_Event e;
            while (SDL_PollEvent(&e))
            {
                switch (e.type)
                {
                case SDL_QUIT:    running = false; break;
                case SDL_KEYDOWN: Updatekey(&e.key, 1); break;
                case SDL_KEYUP: Updatekey(&e.key, 0); break;
                case SDL_WINDOWEVENT:
                    switch (e.window.event)
                    {
                        case SDL_WINDOWEVENT_EXPOSED: break;
                        case SDL_WINDOWEVENT_RESIZED: resize(&e.window); break;
                    }
                }
            }
            display();
        }

        SDL_Quit();
    }

private:
    SDL_Window *window;
    SDL_GLContext glcontext;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    Chip8 & chip8;
};

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

    emu.Dump();
    printf("Starting emulation...\n");

#ifdef DEBUG
    emu.Debug();
#else
    emu.Run();
#endif

    return 0;
}
