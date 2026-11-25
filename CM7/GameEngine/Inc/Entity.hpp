#pragma once
#include "Display.hpp"
#include "Sprites.hpp"

class Entity {
protected:
    int x, y;
    int width, height;
    bool alive;
    const Sprite* sprite;
    uint16_t color;

public:
    Entity(const Sprite* sprite, int x, int y);
    Entity(int w, int h, int x, int y, uint16_t color = COLOR_WHITE);
    virtual ~Entity() = default;

    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int nx) { x = nx; }
    void setY(int ny) { y = ny; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isAlive() const { return alive; }
    void kill() { alive = false; }
    void setAlive(bool a) { alive = a; }

    void setSprite(const Sprite* s);
    const Sprite* getSprite() const { return sprite; }

    void move(int dx, int dy) { x += dx; y += dy; }
    bool collides(const Entity& other) const;

    virtual void render(Display& display);
};
