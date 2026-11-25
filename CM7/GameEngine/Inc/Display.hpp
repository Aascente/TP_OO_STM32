#pragma once
#include <cstdint>
#include <cstddef>
#include "Sprites.hpp"
#include "SharedArcade.hpp"

#define DISPLAY_WIDTH   480
#define DISPLAY_HEIGHT  480

// Standard RGB565 Colors
#define COLOR_BLACK      0x0000
#define COLOR_WHITE      0xFFFF
#define COLOR_RED        0xF800
#define COLOR_GREEN      0x07E0
#define COLOR_BLUE       0x001F
#define COLOR_YELLOW     0xFFE0
#define COLOR_CYAN       0x07FF
#define COLOR_GRAY_DARK  0x31A6
#define COLOR_GRAY_MID   0x6B4D
#define COLOR_GRAY_LIGHT 0xCE79

class Display {
private:
    uint16_t* framebuffer;
    bool allocatedInternally;
    RenderSnapshot pendingSnapshot;

public:
    Display(uint16_t* externalBuffer = nullptr);
    ~Display();

    void init();
    void setFrameMeta(ScreenMode mode, uint8_t gameState, uint8_t selectedMenu,
                      int32_t score, uint8_t lives, uint8_t hasShield);

    void clear(uint16_t color = COLOR_BLACK);
    void drawPixel(int x, int y, uint16_t color);
    void drawRect(int x, int y, int w, int h, uint16_t color);
    void fillRect(int x, int y, int w, int h, uint16_t color);
    void drawSprite(int x, int y, const Sprite& sprite);
    void drawGradientBackground(uint16_t topColor, uint16_t botColor, bool drawGrid = false, int gridSize = 20);

    void drawChar(int x, int y, char c, uint16_t color, uint16_t bg = CHROMA_KEY, int scale = 1);
    void drawString(int x, int y, const char* str, uint16_t color, uint16_t bg = CHROMA_KEY, int scale = 1);

    // CM7: publishes RenderSnapshot to SRAM4 via HSEM for CM4
    // CM4: blits offscreen SDRAM buffer to LTDC Layer 0 (0xD0000000)
    void present();

    uint16_t* getBuffer() { return framebuffer; }
    int getWidth() const { return DISPLAY_WIDTH; }
    int getHeight() const { return DISPLAY_HEIGHT; }
};
