#pragma once
#include <cstdint>

struct InputState {
    bool up;
    bool down;
    bool left;
    bool right;
    bool sel;
    bool b2; // Button B2 (Cannon shoot / action)
    bool b1; // Button B1 (User button)
};

class InputController {
private:
    InputState currentState;
    InputState previousState;
    InputState justPressedState;

public:
    InputController();
    void init();
    void update();

    const InputState& getState() const { return currentState; }

    bool isUp() const { return currentState.up; }
    bool isDown() const { return currentState.down; }
    bool isLeft() const { return currentState.left; }
    bool isRight() const { return currentState.right; }
    bool isSelect() const { return currentState.sel; }
    bool isB2() const { return currentState.b2; }

    bool isJustPressedUp() const { return justPressedState.up; }
    bool isJustPressedDown() const { return justPressedState.down; }
    bool isJustPressedLeft() const { return justPressedState.left; }
    bool isJustPressedRight() const { return justPressedState.right; }
    bool isJustPressedSelect() const { return justPressedState.sel; }
    bool isJustPressedB2() const { return justPressedState.b2; }
};
