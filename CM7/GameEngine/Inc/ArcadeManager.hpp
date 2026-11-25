#pragma once
#include "Display.hpp"
#include "InputController.hpp"
#include "Game.hpp"
#include "GameFactory.hpp"
#include <memory>

class ArcadeManager {
public:
    enum class AppState { MENU, PLAYING };

private:
    AppState appState;
    int selectedGameIndex;
    std::unique_ptr<Game> activeGame;
    Display display;
    InputController input;
    int returnToMenuCounter;

public:
    ArcadeManager();
    void init();
    void loop();

private:
    void updateMenu();
    void renderMenu();
    void launchGame(GameFactory::TypeGame type);
    void returnToMenu();
};
