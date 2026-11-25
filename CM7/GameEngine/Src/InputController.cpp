#include "InputController.hpp"

#if defined(USE_STM32H747I_DISCO)
#include "stm32h747i_discovery.h"
#define HAS_BSP_INPUT 1
#else
#define HAS_BSP_INPUT 0
#endif

// STM32H747I-DISCO Hardware Mapping (Interrupt / EXTI Mode):
// Joystick B3 (GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP -> active LOW):
//   SEL   : GPIOK, PIN 2 (EXTI2_IRQn)
//   DOWN  : GPIOK, PIN 3 (EXTI3_IRQn)
//   LEFT  : GPIOK, PIN 4 (EXTI4_IRQn)
//   RIGHT : GPIOK, PIN 5 (EXTI9_5_IRQn)
//   UP    : GPIOK, PIN 6 (EXTI9_5_IRQn)
// Button B2 (GPIO_MODE_IT_RISING_FALLING -> active HIGH):
//   B2    : GPIOC, PIN 13 (EXTI15_10_IRQn)

enum InputBit : uint32_t {
    BIT_UP    = 1U << 0,
    BIT_DOWN  = 1U << 1,
    BIT_LEFT  = 1U << 2,
    BIT_RIGHT = 1U << 3,
    BIT_SEL   = 1U << 4,
    BIT_B2    = 1U << 5
};

static volatile uint32_t isrHeldMask  = 0U;
static volatile uint32_t isrLatchMask = 0U;

#if HAS_BSP_INPUT
static volatile uint32_t lastPressTick[6] = {0};
static constexpr uint32_t DEBOUNCE_MS = 20U;

static void handleExtiEdge(uint32_t bitIndex, bool pressed) {
    uint32_t mask = (1U << bitIndex);
    if (pressed) {
        isrHeldMask |= mask;
        uint32_t now = HAL_GetTick();
        if ((now - lastPressTick[bitIndex]) >= DEBOUNCE_MS) {
            isrLatchMask |= mask;
            lastPressTick[bitIndex] = now;
        }
    } else {
        isrHeldMask &= ~mask;
    }
}

extern "C" void BSP_JOY_Callback(JOY_TypeDef JOY, uint32_t JoyPin) {
    if (JOY != JOY1) return;

    switch (JoyPin) {
        case JOY_UP:
            handleExtiEdge(0, HAL_GPIO_ReadPin(JOY1_UP_GPIO_PORT, JOY1_UP_PIN) == GPIO_PIN_RESET);
            break;
        case JOY_DOWN:
            handleExtiEdge(1, HAL_GPIO_ReadPin(JOY1_DOWN_GPIO_PORT, JOY1_DOWN_PIN) == GPIO_PIN_RESET);
            break;
        case JOY_LEFT:
            handleExtiEdge(2, HAL_GPIO_ReadPin(JOY1_LEFT_GPIO_PORT, JOY1_LEFT_PIN) == GPIO_PIN_RESET);
            break;
        case JOY_RIGHT:
            handleExtiEdge(3, HAL_GPIO_ReadPin(JOY1_RIGHT_GPIO_PORT, JOY1_RIGHT_PIN) == GPIO_PIN_RESET);
            break;
        case JOY_SEL:
            handleExtiEdge(4, HAL_GPIO_ReadPin(JOY1_SEL_GPIO_PORT, JOY1_SEL_PIN) == GPIO_PIN_RESET);
            break;
        default:
            break;
    }
}

extern "C" void BSP_PB_Callback(Button_TypeDef Button) {
    if (Button != BUTTON_WAKEUP) return;

    bool pressed = (HAL_GPIO_ReadPin(BUTTON_WAKEUP_GPIO_PORT, BUTTON_WAKEUP_PIN) == GPIO_PIN_SET);
    handleExtiEdge(5, pressed);
}
#endif

InputController::InputController() {
    currentState = { false, false, false, false, false, false, false };
    previousState = currentState;
    justPressedState = currentState;
}

void InputController::init() {
    isrHeldMask = 0U;
    isrLatchMask = 0U;

#if HAS_BSP_INPUT
    // 1. Configure Joystick B3 in EXTI mode (BSP configures GPIO_MODE_IT_RISING_FALLING + NVIC)
    BSP_JOY_Init(JOY1, JOY_MODE_EXTI, JOY_ALL);

    // 2. Configure Button B2 (BUTTON_WAKEUP) in EXTI mode and enable both rising & falling edges
    //    so interrupts fire on both press (rising) and release (falling).
    BSP_PB_Init(BUTTON_WAKEUP, BUTTON_MODE_EXTI);
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin   = BUTTON_WAKEUP_PIN;
    gpio.Mode  = GPIO_MODE_IT_RISING_FALLING;
    gpio.Pull  = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BUTTON_WAKEUP_GPIO_PORT, &gpio);

    // Synchronize initial state once at startup
    uint32_t initMask = 0U;
    if (HAL_GPIO_ReadPin(JOY1_UP_GPIO_PORT,    JOY1_UP_PIN)    == GPIO_PIN_RESET) initMask |= BIT_UP;
    if (HAL_GPIO_ReadPin(JOY1_DOWN_GPIO_PORT,  JOY1_DOWN_PIN)  == GPIO_PIN_RESET) initMask |= BIT_DOWN;
    if (HAL_GPIO_ReadPin(JOY1_LEFT_GPIO_PORT,  JOY1_LEFT_PIN)  == GPIO_PIN_RESET) initMask |= BIT_LEFT;
    if (HAL_GPIO_ReadPin(JOY1_RIGHT_GPIO_PORT, JOY1_RIGHT_PIN) == GPIO_PIN_RESET) initMask |= BIT_RIGHT;
    if (HAL_GPIO_ReadPin(JOY1_SEL_GPIO_PORT,   JOY1_SEL_PIN)   == GPIO_PIN_RESET) initMask |= BIT_SEL;
    if (HAL_GPIO_ReadPin(BUTTON_WAKEUP_GPIO_PORT, BUTTON_WAKEUP_PIN) == GPIO_PIN_SET) initMask |= BIT_B2;
    isrHeldMask = initMask;
#endif
}

void InputController::update() {
    previousState = currentState;

#if HAS_BSP_INPUT
    __disable_irq();
    uint32_t held  = isrHeldMask;
    uint32_t latch = isrLatchMask;
    isrLatchMask = 0U;
    __enable_irq();

    uint32_t active = held | latch;

    currentState.up    = (active & BIT_UP)    != 0U;
    currentState.down  = (active & BIT_DOWN)  != 0U;
    currentState.left  = (active & BIT_LEFT)  != 0U;
    currentState.right = (active & BIT_RIGHT) != 0U;
    currentState.sel   = (active & BIT_SEL)   != 0U;
    currentState.b2    = (active & BIT_B2)    != 0U;
    currentState.b1    = currentState.b2;

    justPressedState.up    = (latch & BIT_UP)    != 0U;
    justPressedState.down  = (latch & BIT_DOWN)  != 0U;
    justPressedState.left  = (latch & BIT_LEFT)  != 0U;
    justPressedState.right = (latch & BIT_RIGHT) != 0U;
    justPressedState.sel   = (latch & BIT_SEL)   != 0U;
    justPressedState.b2    = (latch & BIT_B2)    != 0U;
    justPressedState.b1    = justPressedState.b2;
#else
    justPressedState = { false, false, false, false, false, false, false };
#endif
}
