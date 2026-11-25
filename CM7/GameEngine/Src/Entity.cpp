#include "Entity.hpp"

Entity::Entity(const Sprite* sprite, int x, int y)
    : x(x), y(y), alive(true), sprite(sprite), color(COLOR_WHITE) {
    if (sprite) {
        width = sprite->width;
        height = sprite->height;
    } else {
        width = 20;
        height = 20;
    }
}

Entity::Entity(int w, int h, int x, int y, uint16_t color)
    : x(x), y(y), width(w), height(h), alive(true), sprite(nullptr), color(color) {}

void Entity::setSprite(const Sprite* s) {
    sprite = s;
    if (s) {
        width = s->width;
        height = s->height;
    }
}

bool Entity::collides(const Entity& other) const {
    if (!alive || !other.alive) return false;
    int x1 = x, y1 = y, x2 = x + width, y2 = y + height;
    int ox1 = other.x, oy1 = other.y, ox2 = other.x + other.width, oy2 = other.y + other.height;
    return !(ox1 >= x2 || ox2 <= x1 || oy1 >= y2 || oy2 <= y1);
}

void Entity::render(Display& display) {
    if (!alive) return;
    if (sprite) {
        display.drawSprite(x, y, *sprite);
    } else {
        display.fillRect(x, y, width, height, color);
    }
}
