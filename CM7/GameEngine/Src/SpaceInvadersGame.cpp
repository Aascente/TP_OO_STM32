#include "SpaceInvadersGame.hpp"
#include <cstdlib>

SpaceInvadersGame::SpaceInvadersGame()
    : player(&SPRITE_SI_PLAYER, 216, 430),
      bonus(&SPRITE_SI_BONUS, 0, 0),
      shield(&SPRITE_SI_BUNKER, 0, 0),
      hasShield(false), shieldTimer(0),
      enemyDirection(1), enemySpeed(1),
      enemyStepCounter(0), enemyShootCounter(0),
      screenWidth(DISPLAY_WIDTH), screenHeight(DISPLAY_HEIGHT) {
    bonus.kill();
    shield.kill();
    init();
}

void SpaceInvadersGame::init() {
    player = Entity(&SPRITE_SI_PLAYER, screenWidth / 2 - 23, screenHeight - 50);
    enemies.clear();
    bullets.clear();
    bonus.kill();
    shield.kill();
    hasShield = false;
    shieldTimer = 0;
    enemyDirection = 1;
    enemySpeed = 1;
    enemyStepCounter = 0;
    enemyShootCounter = 0;

    score = 0;
    lives = 3;
    alive = true;
    pause = false;
    state = GameState::PLAY;

    // 5 rows x 8 columns of enemies (40 enemies, balanced for 480px width)
    const Sprite* enemySprites[8] = {
        &SPRITE_SI_ENEMY1, &SPRITE_SI_ENEMY2, &SPRITE_SI_ENEMY3,
        &SPRITE_SI_ENEMY5, &SPRITE_SI_ENEMY6, &SPRITE_SI_ENEMY7,
        &SPRITE_SI_ENEMY8, &SPRITE_SI_ENEMY9
    };

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 8; ++col) {
            int x = col * 45 + 55;
            int y = row * 35 + 50;
            const Sprite* s = enemySprites[(row * 8 + col) % 8];
            Enemy e = { x, y, s->width, s->height, true, s };
            enemies.push_back(e);
        }
    }
}

void SpaceInvadersGame::update(const InputState& input) {
    if (!alive || pause || state != GameState::PLAY) {
        if ((hasLost() || hasWon()) && (input.sel || input.b2)) {
            reset();
        }
        return;
    }

    // Direction on Joystick B3
    if (input.left) moveLeft();
    if (input.right) moveRight();

    // Command of the cannon controlled by Button B2
    if (input.b2) {
        action();
    }

    // Shield follow & timeout
    if (hasShield) {
        shieldTimer++;
        if (shieldTimer >= SHIELD_DURATION_TICKS) {
            deactivateShield();
        } else {
            shield.setX(player.getX() + player.getWidth() / 2 - shield.getWidth() / 2);
            shield.setY(player.getY() - 15);
        }
    }

    updateEnemies();
    updateBullets();
    updateBonus();
    checkCollisions();

    // Check Win
    bool anyEnemyAlive = false;
    for (const auto& e : enemies) {
        if (e.alive) { anyEnemyAlive = true; break; }
    }
    if (!anyEnemyAlive) {
        state = GameState::WIN;
    }

    // Check Lose
    if (!player.isAlive()) {
        deactivateShield();
        loseLife();
        if (lives <= 0) {
            state = GameState::LOST;
            alive = false;
        } else {
            player = Entity(&SPRITE_SI_PLAYER, screenWidth / 2 - 23, screenHeight - 50);
        }
    }
}

void SpaceInvadersGame::moveLeft() {
    if (player.getX() > 3) {
        player.setX(player.getX() - PLAYER_SPEED);
        if (player.getX() < 3) player.setX(3);
    }
}

void SpaceInvadersGame::moveRight() {
    int maxX = screenWidth - 3 - player.getWidth();
    if (player.getX() < maxX) {
        player.setX(player.getX() + PLAYER_SPEED);
        if (player.getX() > maxX) player.setX(maxX);
    }
}

void SpaceInvadersGame::action() {
    // Cannon shoot: limit max active player bullets to 3 on screen
    int activePlayerBullets = 0;
    for (const auto& b : bullets) {
        if (b.alive && b.speed < 0) activePlayerBullets++;
    }
    if (activePlayerBullets < 3) {
        launchPlayerBullet();
    }
}

void SpaceInvadersGame::launchPlayerBullet() {
    if (!player.isAlive()) return;
    int bx = player.getX() + player.getWidth() / 2 - 1;
    int by = player.getY() - 10;
    Bullet b = { bx, by, -6, true, &SPRITE_SI_SHOOT_PLAYER };
    bullets.push_back(b);
}

void SpaceInvadersGame::launchEnemyBullet() {
    std::vector<int> aliveIndices;
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].alive) aliveIndices.push_back((int)i);
    }
    if (aliveIndices.empty()) return;

    int shooterIdx = aliveIndices[rand() % aliveIndices.size()];
    const auto& shooter = enemies[shooterIdx];
    int bx = shooter.x + shooter.width / 2 - 8;
    int by = shooter.y + shooter.height;
    Bullet b = { bx, by, 4, true, &SPRITE_SI_SHOOT_ENEMY };
    bullets.push_back(b);
}

void SpaceInvadersGame::updateEnemies() {
    enemyStepCounter++;
    if (enemyStepCounter < 4) return;
    enemyStepCounter = 0;

    bool moveDown = false;
    for (const auto& e : enemies) {
        if (!e.alive) continue;
        if (enemyDirection > 0 && e.x + e.width >= screenWidth - 10) {
            moveDown = true;
            break;
        } else if (enemyDirection < 0 && e.x <= 10) {
            moveDown = true;
            break;
        }
    }

    if (moveDown) {
        enemyDirection *= -1;
        for (auto& e : enemies) {
            e.y += 15;
            if (e.alive && e.y + e.height >= screenHeight - 60) {
                state = GameState::LOST;
                alive = false;
            }
        }
    } else {
        for (auto& e : enemies) {
            e.x += enemyDirection * 3;
        }
    }

    // Random enemy shoot
    enemyShootCounter++;
    if (enemyShootCounter > 30) {
        if ((rand() % 10) < 4) {
            launchEnemyBullet();
        }
        enemyShootCounter = 0;
    }
}

void SpaceInvadersGame::updateBullets() {
    for (auto& b : bullets) {
        if (!b.alive) continue;
        b.y += b.speed;
        if (b.y < 0 || b.y > screenHeight) {
            b.alive = false;
        }
    }
}

void SpaceInvadersGame::updateBonus() {
    if (!hasShield && !bonus.isAlive()) {
        if ((rand() % 400) == 0) {
            int bx = 30 + rand() % (screenWidth - 60);
            bonus = Entity(&SPRITE_SI_BONUS, bx, 50);
        }
    }

    if (bonus.isAlive()) {
        bonus.setY(bonus.getY() + 2);
        if (bonus.getY() > screenHeight) {
            bonus.kill();
        }
    }
}

void SpaceInvadersGame::checkCollisions() {
    // Bullets collision
    for (auto& b : bullets) {
        if (!b.alive) continue;

        // Player bullet (speed < 0) hitting enemy
        if (b.speed < 0) {
            for (auto& e : enemies) {
                if (e.alive && b.x + 3 >= e.x && b.x <= e.x + e.width &&
                    b.y + 17 >= e.y && b.y <= e.y + e.height) {
                    e.alive = false;
                    b.alive = false;
                    addScore(100);
                    break;
                }
            }
        }
        // Enemy bullet (speed > 0) hitting player or shield
        else {
            if (hasShield && shield.isAlive()) {
                Entity bulletEnt(b.sprite, b.x, b.y);
                if (bulletEnt.collides(shield)) {
                    b.alive = false;
                    deactivateShield();
                    continue;
                }
            }
            Entity bulletEnt(b.sprite, b.x, b.y);
            if (player.isAlive() && bulletEnt.collides(player)) {
                b.alive = false;
                player.kill();
            }
        }
    }

    // Player catching falling bonus
    if (bonus.isAlive() && player.isAlive() && player.collides(bonus)) {
        bonus.kill();
        activateShield();
    }
}

void SpaceInvadersGame::activateShield() {
    hasShield = true;
    shieldTimer = 0;
    shield = Entity(&SPRITE_SI_BUNKER, player.getX() - 20, player.getY() - 15);
}

void SpaceInvadersGame::deactivateShield() {
    hasShield = false;
    shield.kill();
}

void SpaceInvadersGame::reset() {
    init();
}

void SpaceInvadersGame::render(Display& display) {
    display.setFrameMeta(ScreenMode::SPACE_INVADERS, static_cast<uint8_t>(state), 0, score, static_cast<uint8_t>(lives), hasShield ? 1 : 0);

    // Dark Space gradient
    display.drawGradientBackground(COLOR_GRAY_LIGHT, COLOR_GRAY_DARK, false);

    // Enemies
    for (const auto& e : enemies) {
        if (e.alive && e.sprite) {
            display.drawSprite(e.x, e.y, *e.sprite);
        }
    }

    // Bullets
    for (const auto& b : bullets) {
        if (b.alive && b.sprite) {
            display.drawSprite(b.x, b.y, *b.sprite);
        }
    }

    // Bonus & Shield
    if (bonus.isAlive()) bonus.render(display);
    if (hasShield && shield.isAlive()) shield.render(display);

    // Player
    if (player.isAlive()) player.render(display);

    // HUD
    char buf[32];
    display.drawString(10, 10, "SPACE INVADERS", COLOR_WHITE);

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

    if (hasShield) {
        display.drawString(380, 10, "SHIELD ON", COLOR_CYAN);
    }

    if (state == GameState::LOST) {
        display.drawString(170, 220, "GAME OVER", COLOR_RED, CHROMA_KEY, 2);
        display.drawString(130, 250, "Press B2 or SEL to restart", COLOR_WHITE);
    } else if (state == GameState::WIN) {
        display.drawString(180, 220, "YOU WIN!", COLOR_GREEN, CHROMA_KEY, 2);
        display.drawString(130, 250, "Press B2 or SEL to restart", COLOR_WHITE);
    }
}
