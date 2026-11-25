#include "Sprites.hpp"

const Sprite SPRITE_PONG_BALL = { SpriteId::PONG_BALL, 20, 20, nullptr };
const Sprite SPRITE_PONG_PADDLE = { SpriteId::PONG_PADDLE, 60, 10, nullptr };
const Sprite SPRITE_SNAKE_FOOD = { SpriteId::SNAKE_FOOD, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_HEAD_RIGHT = { SpriteId::SNAKE_HEAD_RIGHT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_HEAD_LEFT = { SpriteId::SNAKE_HEAD_LEFT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_HEAD_UP = { SpriteId::SNAKE_HEAD_UP, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_HEAD_DOWN = { SpriteId::SNAKE_HEAD_DOWN, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_BODY_RIGHT = { SpriteId::SNAKE_BODY_RIGHT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_BODY_LEFT = { SpriteId::SNAKE_BODY_LEFT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_BODY_UP = { SpriteId::SNAKE_BODY_UP, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_BODY_DOWN = { SpriteId::SNAKE_BODY_DOWN, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_TAIL_RIGHT = { SpriteId::SNAKE_TAIL_RIGHT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_TAIL_LEFT = { SpriteId::SNAKE_TAIL_LEFT, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_TAIL_UP = { SpriteId::SNAKE_TAIL_UP, 20, 20, nullptr };
const Sprite SPRITE_SNAKE_TAIL_DOWN = { SpriteId::SNAKE_TAIL_DOWN, 20, 20, nullptr };
const Sprite SPRITE_SI_PLAYER = { SpriteId::SI_PLAYER, 47, 22, nullptr };
const Sprite SPRITE_SI_ENEMY1 = { SpriteId::SI_ENEMY1, 36, 24, nullptr };
const Sprite SPRITE_SI_ENEMY2 = { SpriteId::SI_ENEMY2, 37, 28, nullptr };
const Sprite SPRITE_SI_ENEMY3 = { SpriteId::SI_ENEMY3, 24, 26, nullptr };
const Sprite SPRITE_SI_ENEMY5 = { SpriteId::SI_ENEMY5, 25, 26, nullptr };
const Sprite SPRITE_SI_ENEMY6 = { SpriteId::SI_ENEMY6, 18, 20, nullptr };
const Sprite SPRITE_SI_ENEMY7 = { SpriteId::SI_ENEMY7, 34, 26, nullptr };
const Sprite SPRITE_SI_ENEMY8 = { SpriteId::SI_ENEMY8, 25, 24, nullptr };
const Sprite SPRITE_SI_ENEMY9 = { SpriteId::SI_ENEMY9, 31, 25, nullptr };
const Sprite SPRITE_SI_SHOOT_PLAYER = { SpriteId::SI_SHOOT_PLAYER, 3, 17, nullptr };
const Sprite SPRITE_SI_SHOOT_ENEMY = { SpriteId::SI_SHOOT_ENEMY, 17, 34, nullptr };
const Sprite SPRITE_SI_BONUS = { SpriteId::SI_BONUS, 17, 17, nullptr };
const Sprite SPRITE_SI_BUNKER = { SpriteId::SI_BUNKER, 87, 40, nullptr };
const Sprite* getSpriteById(SpriteId id) {
    switch (id) {
        case SpriteId::PONG_BALL: return &SPRITE_PONG_BALL;
        case SpriteId::PONG_PADDLE: return &SPRITE_PONG_PADDLE;
        case SpriteId::SNAKE_FOOD: return &SPRITE_SNAKE_FOOD;
        case SpriteId::SNAKE_HEAD_RIGHT: return &SPRITE_SNAKE_HEAD_RIGHT;
        case SpriteId::SNAKE_HEAD_LEFT: return &SPRITE_SNAKE_HEAD_LEFT;
        case SpriteId::SNAKE_HEAD_UP: return &SPRITE_SNAKE_HEAD_UP;
        case SpriteId::SNAKE_HEAD_DOWN: return &SPRITE_SNAKE_HEAD_DOWN;
        case SpriteId::SNAKE_BODY_RIGHT: return &SPRITE_SNAKE_BODY_RIGHT;
        case SpriteId::SNAKE_BODY_LEFT: return &SPRITE_SNAKE_BODY_LEFT;
        case SpriteId::SNAKE_BODY_UP: return &SPRITE_SNAKE_BODY_UP;
        case SpriteId::SNAKE_BODY_DOWN: return &SPRITE_SNAKE_BODY_DOWN;
        case SpriteId::SNAKE_TAIL_RIGHT: return &SPRITE_SNAKE_TAIL_RIGHT;
        case SpriteId::SNAKE_TAIL_LEFT: return &SPRITE_SNAKE_TAIL_LEFT;
        case SpriteId::SNAKE_TAIL_UP: return &SPRITE_SNAKE_TAIL_UP;
        case SpriteId::SNAKE_TAIL_DOWN: return &SPRITE_SNAKE_TAIL_DOWN;
        case SpriteId::SI_PLAYER: return &SPRITE_SI_PLAYER;
        case SpriteId::SI_ENEMY1: return &SPRITE_SI_ENEMY1;
        case SpriteId::SI_ENEMY2: return &SPRITE_SI_ENEMY2;
        case SpriteId::SI_ENEMY3: return &SPRITE_SI_ENEMY3;
        case SpriteId::SI_ENEMY5: return &SPRITE_SI_ENEMY5;
        case SpriteId::SI_ENEMY6: return &SPRITE_SI_ENEMY6;
        case SpriteId::SI_ENEMY7: return &SPRITE_SI_ENEMY7;
        case SpriteId::SI_ENEMY8: return &SPRITE_SI_ENEMY8;
        case SpriteId::SI_ENEMY9: return &SPRITE_SI_ENEMY9;
        case SpriteId::SI_SHOOT_PLAYER: return &SPRITE_SI_SHOOT_PLAYER;
        case SpriteId::SI_SHOOT_ENEMY: return &SPRITE_SI_SHOOT_ENEMY;
        case SpriteId::SI_BONUS: return &SPRITE_SI_BONUS;
        case SpriteId::SI_BUNKER: return &SPRITE_SI_BUNKER;
        default: return nullptr;
    }
}
