#pragma once
#include "Game.hpp"
#include "PongGame.hpp"
#include "SpaceInvadersGame.hpp"
#include "SnakeGame.hpp"
#include <memory>

class GameFactory {
public:
    enum class TypeGame { PONG, SPACE_INVADERS, SNAKE };

    static std::unique_ptr<Game> createGame(TypeGame type);
};
