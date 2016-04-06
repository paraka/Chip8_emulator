#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

class Chip8;

class Graphics
{
public:
    explicit Graphics(Chip8 &chip8);
    ~Graphics() = default;

private:
    void Init();
    void CleanUp();
    void Updatekey(SDL_KeyboardEvent *e, uint8_t val);
    void expandScreen(unsigned char *from, uint32_t *to);
    // Draw into the emulator window
    void renderTexture();
    void updatePixelsWithCPUData();
    void display();

public:
    void mainLoop();

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    Chip8 & chip8;

private:
    // Display resolution is 64×32 pixels, and color is monochrome.
    static const uint32_t SCREEN_WIDTH = 64;
    static const uint32_t SCREEN_HEIGHT = 32;
    static const uint32_t SCALE_FACTOR = 10;
    static constexpr uint32_t display_width = SCREEN_WIDTH * SCALE_FACTOR;
    static constexpr uint32_t display_height = SCREEN_HEIGHT * SCALE_FACTOR;
    static const uint32_t FRAMES_PER_SECOND = 60;
};

#endif //_GRAPHICS_H_
