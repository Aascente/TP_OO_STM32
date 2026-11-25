#pragma once
#include "Game.hpp"
#include "Entity.hpp"
#include <vector>

class SnakeGame : public Game {
public:
    enum class Direction { UP, DOWN, LEFT, RIGHT };

private:
    struct Segment {
        int x, y;
        const Sprite* sprite;
    };

    std::vector<Segment> snake;
    Entity food;
    Direction currentDirection;
    Direction nextDirection;

    static constexpr int GRID_SIZE = 20;
    static constexpr int INITIAL_MOVE_DELAY = 120;
    static constexpr int MIN_MOVE_DELAY = 80;
    static constexpr int INITIAL_SNAKE_LENGTH = 3;

    int moveDelay;
    int moveCounter;
    int screenWidth;
    int screenHeight;

public:
    SnakeGame();
    virtual ~SnakeGame() = default;

    void init() override;
    void update(const InputState& input) override;
    void render(Display& display) override;
    void reset() override;

    void moveLeft() override { setDirection(Direction::LEFT); }
    void moveRight() override { setDirection(Direction::RIGHT); }
    void moveUp() override { setDirection(Direction::UP); }
    void moveDown() override { setDirection(Direction::DOWN); }
    void setDirection(Direction dir);

private:
    void initSnakeSegments();
    void respawnSnake();
    void handleDeath();
    void move();
    void spawnFood();
    bool isOnSnake(int x, int y) const;
    void updateSnakeImages();
    Direction getSegmentDirection(size_t index) const;

    const Sprite* getHeadSprite(Direction dir) const;
    const Sprite* getBodySprite(Direction dir) const;
    const Sprite* getTailSprite(Direction dir) const;
};
