#include "Display.hpp"
#include "Sprites.hpp"
#include "SharedArcade.hpp"
#include "stm32h7xx_hal.h"
#include "stm32h747i_discovery.h"
#include <cstring>

static Display cm4Display;
static uint32_t lastRenderedSeq = 0U;
static uint32_t ledTickCM4 = 0U;

static void drawScoreAndLives(const RenderSnapshot& snap, const char* title) {
    char buf[32];
    cm4Display.drawString(10, 10, title, COLOR_WHITE);

    int s = snap.score;
    buf[0] = 'S'; buf[1] = 'c'; buf[2] = 'o'; buf[3] = 'r'; buf[4] = 'e'; buf[5] = ':'; buf[6] = ' ';
    int idx = 7;
    if (s <= 0) {
        buf[idx++] = '0';
    } else {
        char tmp[10];
        int t = 0;
        while (s > 0 && t < 10) {
            tmp[t++] = static_cast<char>('0' + (s % 10));
            s /= 10;
        }
        while (t > 0) buf[idx++] = tmp[--t];
    }
    buf[idx] = '\0';
    cm4Display.drawString(10, 24, buf, COLOR_WHITE);

    buf[0] = 'L'; buf[1] = 'i'; buf[2] = 'v'; buf[3] = 'e'; buf[4] = 's'; buf[5] = ':'; buf[6] = ' ';
    buf[7] = static_cast<char>('0' + (snap.lives % 10));
    buf[8] = '\0';
    cm4Display.drawString(10, 38, buf, COLOR_WHITE);
}

static void renderMenuScreen(const RenderSnapshot& snap) {
    cm4Display.drawGradientBackground(0x31A6, 0x0842, false);

    cm4Display.drawString(80, 50, "STM32H747I-DISCO", COLOR_CYAN, CHROMA_KEY, 2);
    cm4Display.drawString(140, 80, "ARCADE RETRO", COLOR_YELLOW, CHROMA_KEY, 2);
    cm4Display.drawString(124, 110, "[DUAL-CORE CM7+CM4]", COLOR_GREEN, CHROMA_KEY, 1);

    cm4Display.drawRect(80, 140, 320, 200, COLOR_WHITE);

    const char* options[3] = {
        "1. PONG",
        "2. SPACE INVADERS",
        "3. SNAKE"
    };

    for (int i = 0; i < 3; ++i) {
        int y = 175 + i * 45;
        if (i == static_cast<int>(snap.selectedMenu)) {
            cm4Display.fillRect(90, y - 5, 300, 30, COLOR_BLUE);
            cm4Display.drawString(100, y + 2, ">", COLOR_YELLOW, CHROMA_KEY, 2);
            cm4Display.drawString(130, y + 2, options[i], COLOR_WHITE, CHROMA_KEY, 2);
        } else {
            cm4Display.drawString(130, y + 2, options[i], COLOR_GRAY_LIGHT, CHROMA_KEY, 2);
        }
    }

    cm4Display.drawString(90, 360, "Joystick B3 : Move & Select", COLOR_WHITE);
    cm4Display.drawString(90, 385, "Button B2   : Start / Cannon Fire", COLOR_YELLOW);
    cm4Display.drawString(70, 420, "Hold B2 + SEL: Return to Main Menu", COLOR_CYAN);
}

static void renderGameScreen(const RenderSnapshot& snap) {
    const char* title = "GAME";
    bool drawGrid = false;

    switch (snap.mode) {
        case ScreenMode::PONG:
            title = "PONG";
            break;
        case ScreenMode::SPACE_INVADERS:
            title = "SPACE INVADERS";
            break;
        case ScreenMode::SNAKE:
            title = "SNAKE";
            drawGrid = true;
            break;
        default:
            break;
    }

    cm4Display.drawGradientBackground(COLOR_GRAY_LIGHT, COLOR_GRAY_DARK, drawGrid, 20);

    // Draw all active entity sprites sent by CM7
    uint16_t count = snap.spriteCount;
    if (count > MAX_RENDER_SPRITES) count = MAX_RENDER_SPRITES;

    for (uint16_t i = 0; i < count; ++i) {
        const Sprite* spr = getSpriteById(snap.sprites[i].id);
        if (spr != nullptr) {
            cm4Display.drawSprite(snap.sprites[i].x, snap.sprites[i].y, *spr);
        }
    }

    drawScoreAndLives(snap, title);

    if (snap.mode == ScreenMode::SPACE_INVADERS && snap.hasShield != 0U) {
        cm4Display.drawString(380, 10, "SHIELD ON", COLOR_CYAN);
    }

    // 0=PLAY, 1=PAUSE, 2=LOST, 3=WIN
    if (snap.gameState == 1U) {
        cm4Display.drawString(195, 220, "PAUSED", COLOR_YELLOW, CHROMA_KEY, 2);
    } else if (snap.gameState == 2U) {
        cm4Display.drawString(170, 210, "GAME OVER", COLOR_RED, CHROMA_KEY, 2);
        cm4Display.drawString(130, 245, "Press B2 or SEL to restart", COLOR_WHITE);
    } else if (snap.gameState == 3U) {
        cm4Display.drawString(180, 210, "YOU WIN!", COLOR_GREEN, CHROMA_KEY, 2);
        cm4Display.drawString(130, 245, "Press B2 or SEL to restart", COLOR_WHITE);
    }
}

extern "C" void CM4_Renderer_Init(void) {
    cm4Display.init();
    lastRenderedSeq = 0U;
    ledTickCM4 = 0U;
}

extern "C" void CM4_Renderer_Loop(void) {
    SharedArcadeMemory* shm = SHARED_ARCADE;
    if (shm->magic != ARCADE_MAGIC) {
        return;
    }

    uint32_t seq = shm->frameSeq;
    if (seq == lastRenderedSeq) {
        return;
    }

    // Lock HSEM_DATA_LOCK briefly to copy the 600-byte RenderSnapshot into local CM4 RAM
    if (HAL_HSEM_FastTake(HSEM_DATA_LOCK) != HAL_OK) {
        return;
    }

    RenderSnapshot localSnap;
    std::memcpy(&localSnap, const_cast<const RenderSnapshot*>(&shm->snapshot), sizeof(RenderSnapshot));
    uint32_t currentSeq = shm->frameSeq;
    HAL_HSEM_Release(HSEM_DATA_LOCK, 0U);

    if (localSnap.mode == ScreenMode::MENU) {
        renderMenuScreen(localSnap);
    } else {
        renderGameScreen(localSnap);
    }

    cm4Display.present();
    lastRenderedSeq = currentSeq;
    shm->cm4FramesDrawn = shm->cm4FramesDrawn + 1U;

    // Blue LED heartbeat on CM4 every 30 rendered frames
    if (++ledTickCM4 >= 30U) {
        HAL_GPIO_TogglePin(LED4_GPIO_PORT, LED4_PIN);
        ledTickCM4 = 0U;
    }
}
