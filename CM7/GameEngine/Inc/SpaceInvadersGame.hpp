#pragma once
#include "Game.hpp"
#include "Entity.hpp"
#include <vector>

class SpaceInvadersGame : public Game {
private:
    struct Bullet {
        int x, y;
        int speed; // negative = player bullet (moving up), positive = enemy bullet (moving down)
        bool alive;
        const Sprite* sprite;
    };

    struct Enemy {
        int x, y;
        int width, height;
        bool alive;
        const Sprite* sprite;
    };

    Entity player;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    Entity bonus;
    Entity shield;
    bool hasShield;
    uint32_t shieldTimer;
    static constexpr uint32_t SHIELD_DURATION_TICKS = 600; // ~20 seconds at 30Hz

    int enemyDirection; // 1 = right, -1 = left
    int enemySpeed;
    int enemyStepCounter;
    int enemyShootCounter;

    int screenWidth;
    int screenHeight;
    static constexpr int PLAYER_SPEED = 6;

public:
    SpaceInvadersGame();
    virtual ~SpaceInvadersGame() = default;

    void init() override;
    void update(const InputState& input) override;
    void render(Display& display) override;
    void reset() override;

    void moveLeft() override;
    void moveRight() override;
    void action() override; // Cannon shoot controlled by Button B2

private:
    void updateBullets();
    void updateEnemies();
    void updateBonus();
    void checkCollisions();
    void launchPlayerBullet();
    void launchEnemyBullet();
    void activateShield();
    void deactivateShield();
};
