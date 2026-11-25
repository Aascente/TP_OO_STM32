#pragma once
#include "Game.hpp"
#include "Entity.hpp"

class PongGame : public Game {
private:
    Entity ball;
    Entity paddle;
    int speedX;
    int speedY;
    int bounceCount;
    static constexpr int WIN_BOUNCES = 5;
    static constexpr int PADDLE_SPEED = 5;
    int arenaWidth;
    int arenaHeight;

public:
    PongGame();
    virtual ~PongGame() = default;

    void init() override;
    void update(const InputState& input) override;
    void render(Display& display) override;
    void reset() override;

    void moveLeft() override;
    void moveRight() override;
    void action() override;

private:
    void moveBall();
};
