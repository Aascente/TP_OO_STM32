#include "GameFactory.hpp"

std::unique_ptr<Game> GameFactory::createGame(TypeGame type) {
    switch (type) {
        case TypeGame::PONG:
            return std::make_unique<PongGame>();
        case TypeGame::SPACE_INVADERS:
            return std::make_unique<SpaceInvadersGame>();
        case TypeGame::SNAKE:
            return std::make_unique<SnakeGame>();
    }
    return nullptr;
}
