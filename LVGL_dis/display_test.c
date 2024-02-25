//
// Created by ozcom on 2022/5/14.
//

#include "display_test.h"

static uint8_t index = 0;

static const uint16_t colors[] = {
        BLACK,
        BLUE,
        BRED,
        GRED,
        GBLUE,
        RED,
        MAGENTA,
        GREEN,
        CYAN,
        YELLOW,
        BROWN,
        BRRED,
        GRAY,
        DARKBLUE,
        LIGHTBLUE,
        GRAYBLUE,
        LIGHTGREEN,
        LGRAY,
        LGRAYBLUE,
        LBBLUE,
        WHITE
};


void DISPLAY_test(void) {

    DISPLAY_Fill(30, 30, DISPLAY_W - 30, DISPLAY_H - 30, colors[index]);
    index++;
    index %= (sizeof(colors) - 1);

//    HAL_Delay(1000);

}
