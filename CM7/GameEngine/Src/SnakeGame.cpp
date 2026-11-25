#include "SnakeGame.hpp"
#include <cstdlib>

SnakeGame::SnakeGame()
    : food(&SPRITE_SNAKE_FOOD, 0, 0),
      currentDirection(Direction::RIGHT), nextDirection(Direction::RIGHT),
      moveDelay(INITIAL_MOVE_DELAY), moveCounter(0),
      screenWidth(DISPLAY_WIDTH), screenHeight(DISPLAY_HEIGHT) {
    init();
}

void SnakeGame::init() {
    snake.clear();
    initSnakeSegments();

    currentDirection = Direction::RIGHT;
    nextDirection = Direction::RIGHT;
    moveCounter = 0;
    moveDelay = INITIAL_MOVE_DELAY;

    score = 0;
    lives = 3;
    alive = true;
    pause = false;
    state = GameState::PLAY;

    spawnFood();
}

void SnakeGame::initSnakeSegments() {
    int startX = (screenWidth / (2 * GRID_SIZE)) * GRID_SIZE;
    int startY = (screenHeight / (2 * GRID_SIZE)) * GRID_SIZE;

    // Snake facing RIGHT: Head at startX, Body at startX-20, Tail at startX-40
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
        int x = startX - i * GRID_SIZE;
        int y = startY;
        const Sprite* s = nullptr;
        if (i == 0) s = getHeadSprite(Direction::RIGHT);
        else if (i == INITIAL_SNAKE_LENGTH - 1) s = getTailSprite(Direction::RIGHT);
        else s = getBodySprite(Direction::RIGHT);

        Segment seg = { x, y, s };
        snake.push_back(seg);
    }
}

void SnakeGame::respawnSnake() {
    snake.clear();
    initSnakeSegments();
    currentDirection = Direction::RIGHT;
    nextDirection = Direction::RIGHT;
    moveCounter = 0;
    moveDelay = INITIAL_MOVE_DELAY;
}

void SnakeGame::handleDeath() {
    loseLife();
    if (lives > 0) {
        respawnSnake();
    } else {
        alive = false;
        state = GameState::LOST;
    }
}

void SnakeGame::setDirection(Direction dir) {
    // Prevent immediate reverse turns
    switch (currentDirection) {
        case Direction::UP:    if (dir == Direction::DOWN) return; break;
        case Direction::DOWN:  if (dir == Direction::UP) return; break;
        case Direction::LEFT:  if (dir == Direction::RIGHT) return; break;
        case Direction::RIGHT: if (dir == Direction::LEFT) return; break;
    }
    nextDirection = dir;
}

void SnakeGame::update(const InputState& input) {
    if (!alive || pause || state != GameState::PLAY) {
        if ((hasLost() || hasWon()) && (input.sel || input.b2)) {
            reset();
        }
        return;
    }

    // Direction from Joystick B3
    if (input.up) moveUp();
    else if (input.down) moveDown();
    else if (input.left) moveLeft();
    else if (input.right) moveRight();

    // Game loop timing (16ms per update tick)
    moveCounter += 16;
    if (moveCounter >= moveDelay) {
        move();
        moveCounter -= moveDelay;
    }
}

void SnakeGame::move() {
    if (snake.empty() || state != GameState::PLAY || !alive) return;

    currentDirection = nextDirection;
    int newX = snake[0].x;
    int newY = snake[0].y;

    switch (currentDirection) {
        case Direction::UP:    newY -= GRID_SIZE; break;
        case Direction::DOWN:  newY += GRID_SIZE; break;
        case Direction::LEFT:  newX -= GRID_SIZE; break;
        case Direction::RIGHT: newX += GRID_SIZE; break;
    }

    // Border walls collision
    if (newX < 0 || newX >= screenWidth || newY < 0 || newY >= screenHeight) {
        handleDeath();
        return;
    }

    // Check Food collision
    bool ateFood = false;
    if (food.isAlive() && (newX == food.getX() && newY == food.getY())) {
        ateFood = true;
        food.kill();
        addScore(10);
        if (moveDelay > MIN_MOVE_DELAY) moveDelay -= 1; // Gentle speedup (-1ms)
        spawnFood();
    }

    // Self body collision
    size_t checkLimit = ateFood ? snake.size() : snake.size() - 1;
    for (size_t i = 1; i < checkLimit; ++i) {
        if (snake[i].x == newX && snake[i].y == newY) {
            handleDeath();
            return;
        }
    }

    // Add new head
    Segment newHead = { newX, newY, getHeadSprite(currentDirection) };
    snake.insert(snake.begin(), newHead);

    // If not eaten, remove old tail
    if (!ateFood) {
        snake.pop_back();
    }

    updateSnakeImages();

    // Win condition (filled half the board)
    int totalCells = (screenWidth / GRID_SIZE) * (screenHeight / GRID_SIZE);
    if ((int)snake.size() >= totalCells / 2) {
        state = GameState::WIN;
    }
}

void SnakeGame::spawnFood() {
    int minCol = 1;
    int maxCol = (screenWidth / GRID_SIZE) - 2;
    int minRow = 3; // below HUD
    int maxRow = (screenHeight / GRID_SIZE) - 2;

    int x = 0, y = 0;
    int attempts = 0;
    do {
        x = (minCol + rand() % (maxCol - minCol + 1)) * GRID_SIZE;
        y = (minRow + rand() % (maxRow - minRow + 1)) * GRID_SIZE;
        attempts++;
        if (attempts > 300) break;
    } while (isOnSnake(x, y));

    food = Entity(&SPRITE_SNAKE_FOOD, x, y);
}

bool SnakeGame::isOnSnake(int x, int y) const {
    for (const auto& seg : snake) {
        if (seg.x == x && seg.y == y) return true;
    }
    return false;
}

void SnakeGame::updateSnakeImages() {
    if (snake.empty()) return;
    for (size_t i = 0; i < snake.size(); ++i) {
        Direction dir = getSegmentDirection(i);
        if (i == 0) {
            snake[i].sprite = getHeadSprite(dir);
        } else if (i == snake.size() - 1) {
            snake[i].sprite = getTailSprite(dir);
        } else {
            snake[i].sprite = getBodySprite(dir);
        }
    }
}

SnakeGame::Direction SnakeGame::getSegmentDirection(size_t index) const {
    if (index == 0) return currentDirection;
    int dx = snake[index - 1].x - snake[index].x;
    int dy = snake[index - 1].y - snake[index].y;
    if (dx > 0) return Direction::RIGHT;
    if (dx < 0) return Direction::LEFT;
    if (dy > 0) return Direction::DOWN;
    if (dy < 0) return Direction::UP;
    return currentDirection;
}

const Sprite* SnakeGame::getHeadSprite(Direction dir) const {
    switch (dir) {
        case Direction::RIGHT: return &SPRITE_SNAKE_HEAD_RIGHT;
        case Direction::LEFT:  return &SPRITE_SNAKE_HEAD_LEFT;
        case Direction::UP:    return &SPRITE_SNAKE_HEAD_UP;
        case Direction::DOWN:  return &SPRITE_SNAKE_HEAD_DOWN;
    }
    return &SPRITE_SNAKE_HEAD_RIGHT;
}

const Sprite* SnakeGame::getBodySprite(Direction dir) const {
    switch (dir) {
        case Direction::RIGHT: return &SPRITE_SNAKE_BODY_RIGHT;
        case Direction::LEFT:  return &SPRITE_SNAKE_BODY_LEFT;
        case Direction::UP:    return &SPRITE_SNAKE_BODY_UP;
        case Direction::DOWN:  return &SPRITE_SNAKE_BODY_DOWN;
    }
    return &SPRITE_SNAKE_BODY_RIGHT;
}

const Sprite* SnakeGame::getTailSprite(Direction dir) const {
    switch (dir) {
        case Direction::RIGHT: return &SPRITE_SNAKE_TAIL_RIGHT;
        case Direction::LEFT:  return &SPRITE_SNAKE_TAIL_LEFT;
        case Direction::UP:    return &SPRITE_SNAKE_TAIL_UP;
        case Direction::DOWN:  return &SPRITE_SNAKE_TAIL_DOWN;
    }
    return &SPRITE_SNAKE_TAIL_RIGHT;
}

void SnakeGame::reset() {
    init();
}

void SnakeGame::render(Display& display) {
    // Grey gradient with 20px grid
    display.drawGradientBackground(COLOR_GRAY_LIGHT, COLOR_GRAY_DARK, true, GRID_SIZE);

    // Food
    if (food.isAlive()) food.render(display);

    // Snake segments
    for (const auto& seg : snake) {
        if (seg.sprite) {
            display.drawSprite(seg.x, seg.y, *seg.sprite);
        }
    }

    // HUD
    char buf[32];
    display.drawString(10, 10, "SNAKE", COLOR_WHITE);

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

    buf[0] = 'L'; buf[1] = 'i'; buf[2] = 'v'; buf[3] = 'e'; buf[4] = 's'; buf[5] = ':'; buf[6] = ' ';
    buf[7] = '0' + lives; buf[8] = '\0';
    display.drawString(10, 38, buf, COLOR_WHITE);

    if (state == GameState::LOST) {
        display.drawString(170, 220, "GAME OVER", COLOR_RED, CHROMA_KEY, 2);
        display.drawString(130, 250, "Press B2 or SEL to restart", COLOR_WHITE);
    } else if (state == GameState::WIN) {
        display.drawString(180, 220, "YOU WIN!", COLOR_GREEN, CHROMA_KEY, 2);
        display.drawString(130, 250, "Press B2 or SEL to restart", COLOR_WHITE);
    }
}
