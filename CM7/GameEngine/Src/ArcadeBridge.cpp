#include "ArcadeManager.hpp"
#include <cstdlib>

#if defined(USE_STM32H747I_DISCO)
#include "stm32h7xx_hal.h"
#endif

static ArcadeManager arcade;

extern "C" void Arcade_Init(void) {
#if defined(USE_STM32H747I_DISCO)
    std::srand(HAL_GetTick());
#endif
    arcade.init();
}

extern "C" void Arcade_Loop(void) {
    arcade.loop();
}
