#pragma once
#include <cstdint>

// D3 SRAM4 (64 KB) shared between Cortex-M7 and Cortex-M4 at identical address
#define SHARED_ARCADE_ADDR   0x38000000U
#define ARCADE_MAGIC         0x41524344U // 'ARCD'

// Hardware Semaphores (HSEM)
#define HSEM_BOOT_SYNC       0U // CM7 -> CM4 boot wakeup after clocks/SDRAM/LCD init
#define HSEM_DATA_LOCK       1U // Mutual exclusion for RenderSnapshot in SRAM4
#define HSEM_FRAME_NOTIFY    2U // Optional CM7 -> CM4 frame-ready notification

// LCD framebuffer constants (configured by CM7's BSP_LCD_Init, written by CM4)
#define SHARED_LCD_LAYER0_ADDR    0xD0000000U
#define SHARED_OFFSCREEN_FB_ADDR  0xD0400000U
#define SHARED_LCD_WIDTH          800
#define SHARED_LCD_HEIGHT         480
#define SHARED_PIXEL_FMT_RGB565   0U
#define SHARED_PIXEL_FMT_ARGB8888 1U

#define MAX_RENDER_SPRITES   96

enum class ScreenMode : uint8_t {
    MENU = 0,
    PONG = 1,
    SPACE_INVADERS = 2,
    SNAKE = 3
};

enum class SpriteId : uint8_t {
    NONE = 0,
    PONG_BALL,
    PONG_PADDLE,
    SNAKE_FOOD,
    SNAKE_HEAD_RIGHT,
    SNAKE_HEAD_LEFT,
    SNAKE_HEAD_UP,
    SNAKE_HEAD_DOWN,
    SNAKE_BODY_RIGHT,
    SNAKE_BODY_LEFT,
    SNAKE_BODY_UP,
    SNAKE_BODY_DOWN,
    SNAKE_TAIL_RIGHT,
    SNAKE_TAIL_LEFT,
    SNAKE_TAIL_UP,
    SNAKE_TAIL_DOWN,
    SI_PLAYER,
    SI_ENEMY1,
    SI_ENEMY2,
    SI_ENEMY3,
    SI_ENEMY5,
    SI_ENEMY6,
    SI_ENEMY7,
    SI_ENEMY8,
    SI_ENEMY9,
    SI_SHOOT_PLAYER,
    SI_SHOOT_ENEMY,
    SI_BONUS,
    SI_BUNKER
};

struct RenderSpriteCmd {
    int16_t x;
    int16_t y;
    SpriteId id;
    uint8_t reserved;
};

struct RenderSnapshot {
    ScreenMode mode;
    uint8_t gameState;     // 0=PLAY, 1=PAUSE, 2=LOST, 3=WIN
    uint8_t selectedMenu;  // 0=Pong, 1=Space Invaders, 2=Snake
    uint8_t lives;
    uint8_t hasShield;
    uint8_t reserved[3];
    int32_t score;
    uint16_t spriteCount;
    uint16_t pad;
    RenderSpriteCmd sprites[MAX_RENDER_SPRITES];
};

struct SharedArcadeMemory {
    volatile uint32_t magic;
    volatile uint32_t lcdPixelFormat;
    volatile uint32_t frameSeq;
    volatile uint32_t cm4FramesDrawn;
    RenderSnapshot snapshot;
};

#define SHARED_ARCADE (reinterpret_cast<SharedArcadeMemory*>(SHARED_ARCADE_ADDR))
