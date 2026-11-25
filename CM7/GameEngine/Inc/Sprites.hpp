#pragma once
#include <cstdint>
#include "SharedArcade.hpp"

#define CHROMA_KEY 0xF81F

struct Sprite {
    SpriteId id;
    int width;
    int height;
    const uint16_t* data;
};

const Sprite* getSpriteById(SpriteId id);

extern const Sprite SPRITE_PONG_BALL;
extern const Sprite SPRITE_PONG_PADDLE;
extern const Sprite SPRITE_SNAKE_FOOD;
extern const Sprite SPRITE_SNAKE_HEAD_RIGHT;
extern const Sprite SPRITE_SNAKE_HEAD_LEFT;
extern const Sprite SPRITE_SNAKE_HEAD_UP;
extern const Sprite SPRITE_SNAKE_HEAD_DOWN;
extern const Sprite SPRITE_SNAKE_BODY_RIGHT;
extern const Sprite SPRITE_SNAKE_BODY_LEFT;
extern const Sprite SPRITE_SNAKE_BODY_UP;
extern const Sprite SPRITE_SNAKE_BODY_DOWN;
extern const Sprite SPRITE_SNAKE_TAIL_RIGHT;
extern const Sprite SPRITE_SNAKE_TAIL_LEFT;
extern const Sprite SPRITE_SNAKE_TAIL_UP;
extern const Sprite SPRITE_SNAKE_TAIL_DOWN;
extern const Sprite SPRITE_SI_PLAYER;
extern const Sprite SPRITE_SI_ENEMY1;
extern const Sprite SPRITE_SI_ENEMY2;
extern const Sprite SPRITE_SI_ENEMY3;
extern const Sprite SPRITE_SI_ENEMY5;
extern const Sprite SPRITE_SI_ENEMY6;
extern const Sprite SPRITE_SI_ENEMY7;
extern const Sprite SPRITE_SI_ENEMY8;
extern const Sprite SPRITE_SI_ENEMY9;
extern const Sprite SPRITE_SI_SHOOT_PLAYER;
extern const Sprite SPRITE_SI_SHOOT_ENEMY;
extern const Sprite SPRITE_SI_BONUS;
extern const Sprite SPRITE_SI_BUNKER;
