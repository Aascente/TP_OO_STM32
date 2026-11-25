#include "Display.hpp"
#include <cstring>
#include <cstdlib>

#if defined(USE_STM32H747I_DISCO)
#include "stm32h7xx_hal.h"
#endif

#if defined(CORE_CM7)
// ============================================================================
// CORTEX-M7: Command Collector -> Sends RenderSnapshot to CM4 via SRAM4 + HSEM
// ============================================================================

Display::Display(uint16_t* externalBuffer)
    : framebuffer(externalBuffer), allocatedInternally(false) {
    std::memset(&pendingSnapshot, 0, sizeof(pendingSnapshot));
}

Display::~Display() {}

void Display::init() {
    std::memset(&pendingSnapshot, 0, sizeof(pendingSnapshot));
}

void Display::setFrameMeta(ScreenMode mode, uint8_t gameState, uint8_t selectedMenu,
                           int32_t score, uint8_t lives, uint8_t hasShield) {
    pendingSnapshot.mode = mode;
    pendingSnapshot.gameState = gameState;
    pendingSnapshot.selectedMenu = selectedMenu;
    pendingSnapshot.score = score;
    pendingSnapshot.lives = lives;
    pendingSnapshot.hasShield = hasShield;
    pendingSnapshot.spriteCount = 0;
}

void Display::clear(uint16_t) {}
void Display::drawPixel(int, int, uint16_t) {}
void Display::drawRect(int, int, int, int, uint16_t) {}
void Display::fillRect(int, int, int, int, uint16_t) {}
void Display::drawGradientBackground(uint16_t, uint16_t, bool, int) {}
void Display::drawChar(int, int, char, uint16_t, uint16_t, int) {}
void Display::drawString(int, int, const char*, uint16_t, uint16_t, int) {}

void Display::drawSprite(int x, int y, const Sprite& sprite) {
    if (pendingSnapshot.spriteCount < MAX_RENDER_SPRITES && sprite.id != SpriteId::NONE) {
        RenderSpriteCmd& cmd = pendingSnapshot.sprites[pendingSnapshot.spriteCount++];
        cmd.x = static_cast<int16_t>(x);
        cmd.y = static_cast<int16_t>(y);
        cmd.id = sprite.id;
        cmd.reserved = 0;
    }
}

void Display::present() {
#if defined(USE_STM32H747I_DISCO)
    // Lock HSEM_DATA_LOCK to safely publish snapshot to D3 SRAM4 (0x38000000)
    while (HAL_HSEM_FastTake(HSEM_DATA_LOCK) != HAL_OK) {}

    SharedArcadeMemory* shm = SHARED_ARCADE;
    shm->snapshot = pendingSnapshot;
    __DMB();
    shm->frameSeq = shm->frameSeq + 1U;
    __DMB();

    HAL_HSEM_Release(HSEM_DATA_LOCK, 0U);

    // Signal CM4 via HSEM_FRAME_NOTIFY
    if (HAL_HSEM_FastTake(HSEM_FRAME_NOTIFY) == HAL_OK) {
        HAL_HSEM_Release(HSEM_FRAME_NOTIFY, 0U);
    }
#endif
}

// Metadata-only sprite descriptors for CM7 game physics/hitboxes (pixel data lives on CM4)
const Sprite SPRITE_PONG_BALL = { SpriteId::PONG_BALL, 20, 20, nullptr };
const Sprite SPRITE_PONG_PADDLE = { SpriteId::PONG_PADDLE, 60, 10, nullptr };
const Sprite SPRITE_SNAKE_FOOD = { SpriteId::SNAKE_FOOD, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_HEAD_RIGHT = { SpriteId::SNAKE_HEAD_RIGHT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_HEAD_LEFT = { SpriteId::SNAKE_HEAD_LEFT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_HEAD_UP = { SpriteId::SNAKE_HEAD_UP, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_HEAD_DOWN = { SpriteId::SNAKE_HEAD_DOWN, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_BODY_RIGHT = { SpriteId::SNAKE_BODY_RIGHT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_BODY_LEFT = { SpriteId::SNAKE_BODY_LEFT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_BODY_UP = { SpriteId::SNAKE_BODY_UP, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_BODY_DOWN = { SpriteId::SNAKE_BODY_DOWN, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_TAIL_RIGHT = { SpriteId::SNAKE_TAIL_RIGHT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_TAIL_LEFT = { SpriteId::SNAKE_TAIL_LEFT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_TAIL_UP = { SpriteId::SNAKE_TAIL_UP, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_TAIL_DOWN = { SpriteId::SNAKE_TAIL_DOWN, 20, 20, nullptr };
const Sprite SPRITE_SI_PLAYER = { SpriteId::SI_PLAYER, 47, 22, nullptr };
const Sprite SPRITE_SI_ENEMY1 = { SpriteId::SI_ENEMY1, 36, 24, nullptr };
const Sprite SPRITE_SI_ENEMY2 = { SpriteId::SI_ENEMY2, 37, 28, nullptr };
const Sprite SPRITE_SI_ENEMY3 = { SpriteId::SI_ENEMY3, 24, 26, nullptr };
const Sprite SPRITE_SI_ENEMY5 = { SpriteId::SI_ENEMY5, 25, 26, nullptr };
const Sprite SPRITE_SI_ENEMY6 = { SpriteId::SI_ENEMY6, 18, 20, nullptr };
const Sprite SPRITE_SI_ENEMY7 = { SpriteId::SI_ENEMY7, 34, 26, nullptr };
const Sprite SPRITE_SI_ENEMY8 = { SpriteId::SI_ENEMY8, 25, 24, nullptr };
const Sprite SPRITE_SI_ENEMY9 = { SpriteId::SI_ENEMY9, 31, 25, nullptr };
const Sprite SPRITE_SI_SHOOT_PLAYER = { SpriteId::SI_SHOOT_PLAYER, 3, 17, nullptr };
const Sprite SPRITE_SI_SHOOT_ENEMY = { SpriteId::SI_SHOOT_ENEMY, 17, 34, nullptr };
const Sprite SPRITE_SI_BONUS = { SpriteId::SI_BONUS, 17, 17, nullptr };
const Sprite SPRITE_SI_BUNKER = { SpriteId::SI_BUNKER, 87, 40, nullptr };

const Sprite* getSpriteById(SpriteId id) {
    switch (id) {
        case SpriteId::PONG_BALL: return &SPRITE_PONG_BALL;
        case SpriteId::PONG_PADDLE: return &SPRITE_PONG_PADDLE;
        case SpriteId::SNAKE_FOOD: return &SPRITE_SNAKE_FOOD;
        case SpriteId::SNAKE_HEAD_RIGHT: return &SPRITE_SNAKE_HEAD_RIGHT;
        case SpriteId::SNAKE_HEAD_LEFT: return &SPRITE_SNAKE_HEAD_LEFT;
        case SpriteId::SNAKE_HEAD_UP: return &SPRITE_SNAKE_HEAD_UP;
        case SpriteId::SNAKE_HEAD_DOWN: return &SPRITE_SNAKE_HEAD_DOWN;
        case SpriteId::SNAKE_BODY_RIGHT: return &SPRITE_SNAKE_BODY_RIGHT;
        case SpriteId::SNAKE_BODY_LEFT: return &SPRITE_SNAKE_BODY_LEFT;
        case SpriteId::SNAKE_BODY_UP: return &SPRITE_SNAKE_BODY_UP;
        case SpriteId::SNAKE_BODY_DOWN: return &SPRITE_SNAKE_BODY_DOWN;
        case SpriteId::SNAKE_TAIL_RIGHT: return &SPRITE_SNAKE_TAIL_RIGHT;
        case SpriteId::SNAKE_TAIL_LEFT: return &SPRITE_SNAKE_TAIL_LEFT;
        case SpriteId::SNAKE_TAIL_UP: return &SPRITE_SNAKE_TAIL_UP;
        case SpriteId::SNAKE_TAIL_DOWN: return &SPRITE_SNAKE_TAIL_DOWN;
        case SpriteId::SI_PLAYER: return &SPRITE_SI_PLAYER;
        case SpriteId::SI_ENEMY1: return &SPRITE_SI_ENEMY1;
        case SpriteId::SI_ENEMY2: return &SPRITE_SI_ENEMY2;
        case SpriteId::SI_ENEMY3: return &SPRITE_SI_ENEMY3;
        case SpriteId::SI_ENEMY5: return &SPRITE_SI_ENEMY5;
        case SpriteId::SI_ENEMY6: return &SPRITE_SI_ENEMY6;
        case SpriteId::SI_ENEMY7: return &SPRITE_SI_ENEMY7;
        case SpriteId::SI_ENEMY8: return &SPRITE_SI_ENEMY8;
        case SpriteId::SI_ENEMY9: return &SPRITE_SI_ENEMY9;
        case SpriteId::SI_SHOOT_PLAYER: return &SPRITE_SI_SHOOT_PLAYER;
        case SpriteId::SI_SHOOT_ENEMY: return &SPRITE_SI_SHOOT_ENEMY;
        case SpriteId::SI_BONUS: return &SPRITE_SI_BONUS;
        case SpriteId::SI_BUNKER: return &SPRITE_SI_BUNKER;
        default: return nullptr;
    }
}

#else
// ============================================================================
// CORTEX-M4 (or host): Full Software Framebuffer Rasterizer -> SDRAM LCD
// ============================================================================

static const uint8_t font8x8[96][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // 32 ' '
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // 33 '!'
    {0x66,0x66,0x24,0x00,0x00,0x00,0x00,0x00}, // 34 '"'
    {0x6C,0x6C,0xFE,0x6C,0xFE,0x6C,0x6C,0x00}, // 35 '#'
    {0x18,0x7E,0xC0,0x7C,0x06,0xFC,0x18,0x00}, // 36 '$'
    {0x00,0xC6,0xCC,0x18,0x30,0x66,0xC6,0x00}, // 37 '%'
    {0x38,0x6C,0x38,0x76,0xDC,0xCC,0x76,0x00}, // 38 '&'
    {0x18,0x18,0x30,0x00,0x00,0x00,0x00,0x00}, // 39 '\''
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, // 40 '('
    {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, // 41 ')'
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // 42 '*'
    {0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00}, // 43 '+'
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30}, // 44 ','
    {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00}, // 45 '-'
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, // 46 '.'
    {0x06,0x0C,0x18,0x30,0x60,0xC0,0x80,0x00}, // 47 '/'
    {0x3C,0x66,0xC3,0xC3,0xC3,0x66,0x3C,0x00}, // 48 '0'
    {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}, // 49 '1'
    {0x3C,0x66,0x06,0x1C,0x30,0x60,0x7E,0x00}, // 50 '2'
    {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00}, // 51 '3'
    {0x0E,0x1E,0x36,0x66,0x7F,0x06,0x0F,0x00}, // 52 '4'
    {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00}, // 53 '5'
    {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00}, // 54 '6'
    {0x7E,0x66,0x06,0x0C,0x18,0x18,0x18,0x00}, // 55 '7'
    {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}, // 56 '8'
    {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00}, // 57 '9'
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, // 58 ':'
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30}, // 59 ';'
    {0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00}, // 60 '<'
    {0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00}, // 61 '='
    {0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00}, // 62 '>'
    {0x3C,0x66,0x06,0x0C,0x18,0x00,0x18,0x00}, // 63 '?'
    {0x3C,0x66,0x06,0x6E,0x76,0x66,0x3C,0x00}, // 64 '@'
    {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00}, // 65 'A'
    {0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00}, // 66 'B'
    {0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00}, // 67 'C'
    {0x78,0x6C,0x66,0x66,0x66,0x6C,0x78,0x00}, // 68 'D'
    {0x7E,0x60,0x60,0x78,0x60,0x60,0x7E,0x00}, // 69 'E'
    {0x7E,0x60,0x60,0x78,0x60,0x60,0x60,0x00}, // 70 'F'
    {0x3C,0x66,0x60,0x6E,0x66,0x66,0x3C,0x00}, // 71 'G'
    {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}, // 72 'H'
    {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 73 'I'
    {0x0E,0x06,0x06,0x06,0x06,0x66,0x3C,0x00}, // 74 'J'
    {0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00}, // 75 'K'
    {0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00}, // 76 'L'
    {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00}, // 77 'M'
    {0x66,0x76,0x7E,0x7E,0x6E,0x66,0x66,0x00}, // 78 'N'
    {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, // 79 'O'
    {0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00}, // 80 'P'
    {0x3C,0x66,0x66,0x66,0x6A,0x6C,0x36,0x00}, // 81 'Q'
    {0x7C,0x66,0x66,0x7C,0x6C,0x66,0x66,0x00}, // 82 'R'
    {0x3C,0x66,0x60,0x3C,0x06,0x66,0x3C,0x00}, // 83 'S'
    {0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, // 84 'T'
    {0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, // 85 'U'
    {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00}, // 86 'V'
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, // 87 'W'
    {0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00}, // 88 'X'
    {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00}, // 89 'Y'
    {0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00}, // 90 'Z'
    {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00}, // 91 '['
    {0xC0,0x60,0x30,0x18,0x0C,0x06,0x02,0x00}, // 92 '\\'
    {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00}, // 93 ']'
    {0x10,0x38,0x6C,0xC6,0x00,0x00,0x00,0x00}, // 94 '^'
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // 95 '_'
    {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, // 96 '`'
    {0x00,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00}, // 97 'a'
    {0x60,0x60,0x7C,0x66,0x66,0x66,0x7C,0x00}, // 98 'b'
    {0x00,0x00,0x3C,0x66,0x60,0x66,0x3C,0x00}, // 99 'c'
    {0x06,0x06,0x3E,0x66,0x66,0x66,0x3E,0x00}, // 100 'd'
    {0x00,0x00,0x3C,0x66,0x7E,0x60,0x3C,0x00}, // 101 'e'
    {0x1C,0x30,0x30,0x7C,0x30,0x30,0x30,0x00}, // 102 'f'
    {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x3C}, // 103 'g'
    {0x60,0x60,0x7C,0x66,0x66,0x66,0x66,0x00}, // 104 'h'
    {0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00}, // 105 'i'
    {0x0C,0x00,0x1C,0x0C,0x0C,0x0C,0x4C,0x38}, // 106 'j'
    {0x60,0x60,0x66,0x6C,0x78,0x6C,0x66,0x00}, // 107 'k'
    {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 108 'l'
    {0x00,0x00,0x66,0x7F,0x7F,0x6B,0x63,0x00}, // 109 'm'
    {0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00}, // 110 'n'
    {0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00}, // 111 'o'
    {0x00,0x00,0x7C,0x66,0x66,0x7C,0x60,0x60}, // 112 'p'
    {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x07}, // 113 'q'
    {0x00,0x00,0x7C,0x66,0x60,0x60,0x60,0x00}, // 114 'r'
    {0x00,0x00,0x3E,0x60,0x3C,0x06,0x7C,0x00}, // 115 's'
    {0x18,0x18,0x7E,0x18,0x18,0x18,0x0E,0x00}, // 116 't'
    {0x00,0x00,0x66,0x66,0x66,0x66,0x3E,0x00}, // 117 'u'
    {0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00}, // 118 'v'
    {0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00}, // 119 'w'
    {0x00,0x00,0x66,0x3C,0x18,0x3C,0x66,0x00}, // 120 'x'
    {0x00,0x00,0x66,0x66,0x66,0x3E,0x06,0x3C}, // 121 'y'
    {0x00,0x00,0x7E,0x0C,0x18,0x30,0x7E,0x00}, // 122 'z'
    {0x0E,0x18,0x18,0x70,0x18,0x18,0x0E,0x00}, // 123 '{'
    {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00}, // 124 '|'
    {0x70,0x18,0x18,0x0E,0x18,0x18,0x70,0x00}, // 125 '}'
    {0x76,0xDC,0x00,0x00,0x00,0x00,0x00,0x00}, // 126 '~'
    {0x7E,0xFF,0xFF,0xFF,0xFF,0xFF,0x7E,0x00}  // 127 delta
};

#if !defined(USE_STM32H747I_DISCO)
static uint16_t internal_sram_framebuffer[DISPLAY_WIDTH * DISPLAY_HEIGHT];
#endif

Display::Display(uint16_t* externalBuffer) {
    std::memset(&pendingSnapshot, 0, sizeof(pendingSnapshot));
    if (externalBuffer != nullptr) {
        framebuffer = externalBuffer;
        allocatedInternally = false;
    } else {
#if defined(USE_STM32H747I_DISCO)
        framebuffer = reinterpret_cast<uint16_t*>(SHARED_OFFSCREEN_FB_ADDR);
#else
        framebuffer = internal_sram_framebuffer;
#endif
        allocatedInternally = false;
    }
}

Display::~Display() {}

void Display::setFrameMeta(ScreenMode, uint8_t, uint8_t, int32_t, uint8_t, uint8_t) {}

void Display::init() {
    clear(COLOR_BLACK);
#if defined(USE_STM32H747I_DISCO)
    uint32_t fbAddr = SHARED_LCD_LAYER0_ADDR;
    uint32_t totalPixels = SHARED_LCD_WIDTH * SHARED_LCD_HEIGHT;
    if (SHARED_ARCADE->lcdPixelFormat == SHARED_PIXEL_FMT_RGB565) {
        volatile uint16_t* lcd = reinterpret_cast<volatile uint16_t*>(fbAddr);
        for (uint32_t i = 0; i < totalPixels; ++i) {
            lcd[i] = 0x0000U;
        }
    } else {
        volatile uint32_t* lcd = reinterpret_cast<volatile uint32_t*>(fbAddr);
        for (uint32_t i = 0; i < totalPixels; ++i) {
            lcd[i] = 0xFF000000U;
        }
    }
    __DSB();
#endif
}

void Display::clear(uint16_t color) {
    uint32_t pair = (static_cast<uint32_t>(color) << 16U) | color;
    uint32_t* fb32 = reinterpret_cast<uint32_t*>(framebuffer);
    const int count32 = (DISPLAY_WIDTH * DISPLAY_HEIGHT) / 2;
    for (int i = 0; i < count32; ++i) {
        fb32[i] = pair;
    }
}

void Display::drawPixel(int x, int y, uint16_t color) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        framebuffer[y * DISPLAY_WIDTH + x] = color;
    }
}

void Display::drawRect(int x, int y, int w, int h, uint16_t color) {
    for (int i = x; i < x + w; ++i) {
        drawPixel(i, y, color);
        drawPixel(i, y + h - 1, color);
    }
    for (int j = y; j < y + h; ++j) {
        drawPixel(x, j, color);
        drawPixel(x + w - 1, j, color);
    }
}

void Display::fillRect(int x, int y, int w, int h, uint16_t color) {
    int startX = x < 0 ? 0 : x;
    int endX = (x + w > DISPLAY_WIDTH) ? DISPLAY_WIDTH : x + w;
    int startY = y < 0 ? 0 : y;
    int endY = (y + h > DISPLAY_HEIGHT) ? DISPLAY_HEIGHT : y + h;

    for (int j = startY; j < endY; ++j) {
        int rowIdx = j * DISPLAY_WIDTH;
        for (int i = startX; i < endX; ++i) {
            framebuffer[rowIdx + i] = color;
        }
    }
}

void Display::drawSprite(int x, int y, const Sprite& sprite) {
    if (!sprite.data) return;
    int sw = sprite.width;
    int sh = sprite.height;
    const uint16_t* data = sprite.data;

    for (int j = 0; j < sh; ++j) {
        int py = y + j;
        if (py < 0 || py >= DISPLAY_HEIGHT) continue;
        int rowIdx = py * DISPLAY_WIDTH;
        int sRowIdx = j * sw;

        for (int i = 0; i < sw; ++i) {
            int px = x + i;
            if (px < 0 || px >= DISPLAY_WIDTH) continue;
            uint16_t col = data[sRowIdx + i];
            if (col != CHROMA_KEY) {
                framebuffer[rowIdx + px] = col;
            }
        }
    }
}

void Display::drawGradientBackground(uint16_t topColor, uint16_t botColor, bool drawGrid, int gridSize) {
    int rTop = (topColor >> 11) & 0x1F;
    int gTop = (topColor >> 5) & 0x3F;
    int bTop = topColor & 0x1F;

    int rBot = (botColor >> 11) & 0x1F;
    int gBot = (botColor >> 5) & 0x3F;
    int bBot = botColor & 0x1F;

    const uint16_t borderCol = 0x18E3U;
    const uint32_t borderPair = 0x18E318E3U;
    const int rowWords = DISPLAY_WIDTH / 2;

    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        uint16_t* row = &framebuffer[y * DISPLAY_WIDTH];
        uint32_t* row32 = reinterpret_cast<uint32_t*>(row);

        if (y < 3 || y >= DISPLAY_HEIGHT - 3) {
            for (int w = 0; w < rowWords; ++w) {
                row32[w] = borderPair;
            }
            continue;
        }

        int r = rTop + ((rBot - rTop) * y) / (DISPLAY_HEIGHT - 1);
        int g = gTop + ((gBot - gTop) * y) / (DISPLAY_HEIGHT - 1);
        int b = bTop + ((bBot - bTop) * y) / (DISPLAY_HEIGHT - 1);
        uint16_t base = static_cast<uint16_t>((r << 11) | (g << 5) | b);

        int rGrid = (r > 3) ? r - 3 : 0;
        int gGrid = (g > 6) ? g - 6 : 0;
        int bGrid = (b > 3) ? b - 3 : 0;
        uint16_t gridCol = static_cast<uint16_t>((rGrid << 11) | (gGrid << 5) | bGrid);

        bool isYGrid = drawGrid && (gridSize > 0) && (y % gridSize == 0);
        uint16_t fillCol = isYGrid ? gridCol : base;
        uint32_t fillPair = (static_cast<uint32_t>(fillCol) << 16U) | fillCol;

        for (int w = 0; w < rowWords; ++w) {
            row32[w] = fillPair;
        }

        if (drawGrid && !isYGrid && gridSize > 0) {
            for (int x = gridSize; x < DISPLAY_WIDTH - 3; x += gridSize) {
                row[x] = gridCol;
            }
        }

        row[0] = borderCol;
        row[1] = borderCol;
        row[2] = borderCol;
        row[DISPLAY_WIDTH - 3] = borderCol;
        row[DISPLAY_WIDTH - 2] = borderCol;
        row[DISPLAY_WIDTH - 1] = borderCol;
    }
}

void Display::drawChar(int x, int y, char c, uint16_t color, uint16_t bg, int scale) {
    if (c < 32 || c > 127) c = '?';
    const uint8_t* glyph = font8x8[c - 32];

    for (int r = 0; r < 8; ++r) {
        uint8_t row = glyph[r];
        for (int b = 0; b < 8; ++b) {
            bool pixelOn = (row & (0x80 >> b)) != 0;
            if (pixelOn) {
                if (scale == 1) drawPixel(x + b, y + r, color);
                else fillRect(x + b * scale, y + r * scale, scale, scale, color);
            } else if (bg != CHROMA_KEY) {
                if (scale == 1) drawPixel(x + b, y + r, bg);
                else fillRect(x + b * scale, y + r * scale, scale, scale, bg);
            }
        }
    }
}

void Display::drawString(int x, int y, const char* str, uint16_t color, uint16_t bg, int scale) {
    if (!str) return;
    int curX = x;
    while (*str) {
        if (*str == '\n') {
            y += 8 * scale + 2;
            curX = x;
        } else {
            drawChar(curX, y, *str, color, bg, scale);
            curX += 8 * scale;
        }
        str++;
    }
}

void Display::present() {
#if defined(USE_STM32H747I_DISCO)
    uint32_t fbAddr = SHARED_LCD_LAYER0_ADDR;
    const int lcdWidth = SHARED_LCD_WIDTH;
    const int xOffset = (lcdWidth - DISPLAY_WIDTH) / 2;

    if (SHARED_ARCADE->lcdPixelFormat == SHARED_PIXEL_FMT_RGB565) {
        volatile uint32_t* lcd32 = reinterpret_cast<volatile uint32_t*>(fbAddr);
        const int rowWords = DISPLAY_WIDTH / 2;
        const int lcdWords = lcdWidth / 2;
        const int xOffWords = xOffset / 2;
        for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
            const uint32_t* src32 = reinterpret_cast<const uint32_t*>(&framebuffer[y * DISPLAY_WIDTH]);
            volatile uint32_t* dst32 = &lcd32[y * lcdWords + xOffWords];
            for (int w = 0; w < rowWords; ++w) {
                dst32[w] = src32[w];
            }
        }
    } else {
        volatile uint32_t* lcd = reinterpret_cast<volatile uint32_t*>(fbAddr);
        const int rowWords = DISPLAY_WIDTH / 2;
        for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
            const uint32_t* src32 = reinterpret_cast<const uint32_t*>(&framebuffer[y * DISPLAY_WIDTH]);
            volatile uint32_t* dst = &lcd[y * lcdWidth + xOffset];
            for (int w = 0; w < rowWords; ++w) {
                uint32_t pair = src32[w];
                uint32_t c0 = pair & 0xFFFFU;
                uint32_t c1 = pair >> 16U;
                dst[w * 2]     = 0xFF000000U | ((c0 & 0xF800U) << 8U) | ((c0 & 0x07E0U) << 5U) | ((c0 & 0x001FU) << 3U);
                dst[w * 2 + 1] = 0xFF000000U | ((c1 & 0xF800U) << 8U) | ((c1 & 0x07E0U) << 5U) | ((c1 & 0x001FU) << 3U);
            }
        }
    }
    __DSB();
#endif
}

#endif // CORE_CM7
