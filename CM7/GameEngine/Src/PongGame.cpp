#include "PongGame.hpp"
#include <cstdlib>

PongGame::PongGame()
    : ball(&SPRITE_PONG_BALL, 230, 80),
      paddle(&SPRITE_PONG_PADDLE, 210, 430),
      speedX(3), speedY(3), bounceCount(0),
      arenaWidth(DISPLAY_WIDTH), arenaHeight(DISPLAY_HEIGHT) {
    init();
}

void PongGame::init() {
    ball = Entity(&SPRITE_PONG_BALL, arenaWidth / 2 - 10, 80);
    paddle = Entity(&SPRITE_PONG_PADDLE, arenaWidth / 2 - 30, arenaHeight - 50);
    speedX = 3;
    speedY = 3;
    bounceCount = 0;
    score = 0;
    lives = 3;
    alive = true;
    pause = false;
    state = GameState::PLAY;
}

void PongGame::update(const InputState& input) {
    if (!alive || pause || state != GameState::PLAY) {
        if ((hasLost() || hasWon()) && (input.sel || input.b2)) {
            reset();
        }
        return;
    }

    if (input.left) moveLeft();
    if (input.right) moveRight();

    moveBall();

    // Check bottom boundary (missed ball)
    if (ball.getY() > arenaHeight) {
        ball.kill();
        loseLife();
        if (lives > 0) {
            ball = Entity(&SPRITE_PONG_BALL, arenaWidth / 2 - 10, 80);
            bounceCount = 0;
        } else {
            state = GameState::LOST;
        }
    }

    if (bounceCount >= WIN_BOUNCES) {
        state = GameState::WIN;
    }
}

void PongGame::moveBall() {
    if (!ball.isAlive()) return;

    // Bounce on side walls
    if (ball.getX() <= 3) speedX = std::abs(speedX);
    else if (ball.getX() + ball.getWidth() >= arenaWidth - 3) speedX = -std::abs(speedX);

    // Bounce on top wall
    if (ball.getY() <= 3) speedY = std::abs(speedY);

    // Bounce on paddle
    if (paddle.isAlive() && ball.collides(paddle)) {
        speedY = -std::abs(speedY);
        bounceCount++;
        addScore(10);
    }

    ball.move(speedX, speedY);
}

void PongGame::moveLeft() {
    if (paddle.getX() > 3) {
        paddle.setX(paddle.getX() - PADDLE_SPEED);
        if (paddle.getX() < 3) paddle.setX(3);
    }
}

void PongGame::moveRight() {
    int maxX = arenaWidth - 3 - paddle.getWidth();
    if (paddle.getX() < maxX) {
        paddle.setX(paddle.getX() + PADDLE_SPEED);
        if (paddle.getX() > maxX) paddle.setX(maxX);
    }
}

void PongGame::action() {
    togglePlayPause();
}

void PongGame::reset() {
    init();
}

void PongGame::render(Display& display) {
    // Grey gradient background matching Java version
    display.drawGradientBackground(COLOR_GRAY_LIGHT, COLOR_GRAY_DARK, false);

    // Render ball and paddle
    ball.render(display);
    paddle.render(display);

    // HUD
    char buf[32];
    display.drawString(10, 10, "PONG", COLOR_WHITE);
    
    // Score
    int s = score;
    buf[0] = 'S'; buf[1] = 'c'; buf[2] = 'o'; buf[3] = 'r'; buf[4] = 'e'; buf[5] = ':'; buf[6] = ' ';
    int idx = 7;
    if (s == 0) { buf[idx++] = '0'; }
    else {
        char temp[10]; int t = 0;
        while (s > 0) { temp[t++] = '0' + (s % 10); s /= 10; }
        while (t > 0) buf[idx++] = temp[--t];
    }
    buf[idx] = '\0';
    display.drawString(10, 24, buf, COLOR_WHITE);

    // Lives
    buf[0] = 'L'; buf[1] = 'i'; buf[2] = 'v'; buf[3] = 'e'; buf[4] = 's'; buf[5] = ':'; buf[6] = ' ';
    buf[7] = '0' + lives; buf[8] = '\0';
    display.drawString(10, 38, buf, COLOR_WHITE);

    if (state == GameState::LOST) {
        display.drawString(170, 200, "GAME OVER", COLOR_RED, CHROMA_KEY, 2);
        display.drawString(130, 230, "Press B2 or SEL to restart", COLOR_WHITE);
    } else if (state == GameState::WIN) {
        display.drawString(180, 200, "YOU WIN!", COLOR_GREEN, CHROMA_KEY, 2);
        display.drawString(130, 230, "Press B2 or SEL to restart", COLOR_WHITE);
    }
}
