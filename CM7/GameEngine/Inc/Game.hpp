#pragma once
#include "Display.hpp"
#include "InputController.hpp"

class Game {
public:
    enum class GameState { PLAY, PAUSE, LOST, WIN };

protected:
    volatile bool alive;
    bool pause;
    int score;
    int lives;
    GameState state;

public:
    Game() : alive(true), pause(false), score(0), lives(3), state(GameState::PLAY) {}
    virtual ~Game() = default;

    bool isAlive() const { return alive; }
    void kill() { alive = false; }
    bool isPause() const { return pause; }
    void togglePlayPause() {
        pause = !pause;
        state = pause ? GameState::PAUSE : GameState::PLAY;
    }
    GameState getState() const { return state; }
    void setState(GameState s) { state = s; }
    int getScore() const { return score; }
    void addScore(int points) { score += points; }
    int getLives() const { return lives; }
    void loseLife() {
        lives--;
        if (lives <= 0) state = GameState::LOST;
    }
    void addLife() { lives++; }
    bool hasWon() const { return state == GameState::WIN; }
    bool hasLost() const { return state == GameState::LOST; }

    virtual void init() = 0;
    virtual void update(const InputState& input) = 0;
    virtual void render(Display& display) = 0;
    virtual void reset() = 0;

    virtual void moveLeft() {}
    virtual void moveRight() {}
    virtual void moveUp() {}
    virtual void moveDown() {}
    virtual void action() {}
};
