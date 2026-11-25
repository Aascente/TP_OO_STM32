#pragma once
#include <cstdint>
#include <cstddef>
#include "Sprites.hpp"

// Virtual display dimensions (fits directly in STM32H7 internal AXI SRAM: 480x480x2 = 460KB)
#define DISPLAY_WIDTH   480
#define DISPLAY_HEIGHT  480

// Standard RGB565 Colors
#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_BLUE      0x001F
#define COLOR_YELLOW    0xFFE0
#define COLOR_CYAN      0x07FF
#define COLOR_GRAY_DARK 0x31A6
#define COLOR_GRAY_MID  0x6B4D
#define COLOR_GRAY_LIGHT 0xCE79

class Display {
private:
    uint16_t* framebuffer;
    bool allocatedInternally;

public:
    Display(uint16_t* externalBuffer = nullptr);
    ~Display();

    void init();
    void clear(uint16_t color = COLOR_BLACK);
    void drawPixel(int x, int y, uint16_t color);
    void drawRect(int x, int y, int w, int h, uint16_t color);
    void fillRect(int x, int y, int w, int h, uint16_t color);
    void drawSprite(int x, int y, const Sprite& sprite);
    void drawGradientBackground(uint16_t topColor, uint16_t botColor, bool drawGrid = false, int gridSize = 20);
    
    void drawChar(int x, int y, char c, uint16_t color, uint16_t bg = CHROMA_KEY, int scale = 1);
    void drawString(int x, int y, const char* str, uint16_t color, uint16_t bg = CHROMA_KEY, int scale = 1);
    
    // Transfer framebuffer to LCD (BSP or LTDC layer)
    void present();

    uint16_t* getBuffer() { return framebuffer; }
    int getWidth() const { return DISPLAY_WIDTH; }
    int getHeight() const { return DISPLAY_HEIGHT; }
};
