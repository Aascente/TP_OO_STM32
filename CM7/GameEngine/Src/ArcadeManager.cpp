#include "ArcadeManager.hpp"

ArcadeManager::ArcadeManager()
    : appState(AppState::MENU), selectedGameIndex(0),
      activeGame(nullptr), returnToMenuCounter(0) {}

void ArcadeManager::init() {
    display.init();
    input.init();
}

void ArcadeManager::launchGame(GameFactory::TypeGame type) {
    activeGame = GameFactory::createGame(type);
    appState = AppState::PLAYING;
    returnToMenuCounter = 0;
}

void ArcadeManager::returnToMenu() {
    activeGame.reset();
    appState = AppState::MENU;
    returnToMenuCounter = 0;
}

void ArcadeManager::updateMenu() {
    if (input.isJustPressedUp()) {
        selectedGameIndex = (selectedGameIndex - 1 + 3) % 3;
    } else if (input.isJustPressedDown()) {
        selectedGameIndex = (selectedGameIndex + 1) % 3;
    }

    // Select with Button B2 or Joystick SEL
    if (input.isJustPressedB2() || input.isJustPressedSelect()) {
        if (selectedGameIndex == 0) launchGame(GameFactory::TypeGame::PONG);
        else if (selectedGameIndex == 1) launchGame(GameFactory::TypeGame::SPACE_INVADERS);
        else if (selectedGameIndex == 2) launchGame(GameFactory::TypeGame::SNAKE);
    }
}

void ArcadeManager::renderMenu() {
    display.setFrameMeta(ScreenMode::MENU, 0, static_cast<uint8_t>(selectedGameIndex), 0, 0, 0);

    // Elegant dark gradient background
    display.drawGradientBackground(0x31A6, 0x0842, false);

    // Title banner
    display.drawString(80, 50, "STM32H747I-DISCO", COLOR_CYAN, CHROMA_KEY, 2);
    display.drawString(140, 80, "ARCADE RETRO", COLOR_YELLOW, CHROMA_KEY, 2);

    // Menu options box
    display.drawRect(80, 140, 320, 200, COLOR_WHITE);

    const char* options[3] = {
        "1. PONG",
        "2. SPACE INVADERS",
        "3. SNAKE"
    };

    for (int i = 0; i < 3; ++i) {
        int y = 175 + i * 45;
        if (i == selectedGameIndex) {
            display.fillRect(90, y - 5, 300, 30, COLOR_BLUE);
            display.drawString(100, y + 2, ">", COLOR_YELLOW, CHROMA_KEY, 2);
            display.drawString(130, y + 2, options[i], COLOR_WHITE, CHROMA_KEY, 2);
        } else {
            display.drawString(130, y + 2, options[i], COLOR_GRAY_LIGHT, CHROMA_KEY, 2);
        }
    }

    // Controls Guide
    display.drawString(90, 360, "Joystick B3 : Move & Select", COLOR_WHITE);
    display.drawString(90, 385, "Button B2   : Start / Cannon Fire", COLOR_YELLOW);
    display.drawString(70, 420, "Hold B2 + SEL: Return to Main Menu", COLOR_CYAN);
}

void ArcadeManager::loop() {
    input.update();

    if (appState == AppState::MENU) {
        updateMenu();
        renderMenu();
    } else if (appState == AppState::PLAYING && activeGame) {
        // Check for return to menu combo (Hold B2 + SEL for ~1 sec / 30 frames at 30Hz)
        if (input.isB2() && input.isSelect()) {
            returnToMenuCounter++;
            if (returnToMenuCounter >= 30) {
                returnToMenu();
                display.present();
                return;
            }
        } else {
            returnToMenuCounter = 0;
        }

        activeGame->update(input.getState());
        activeGame->render(display);
    }

    display.present();
}
