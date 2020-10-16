/*
    Определение текущего состояния по высоте - и уже от этого возможности управления
*/

#ifndef _ctrl_H
#define _ctrl_H

typedef enum {
    CTRL_INIT = 0,
    CTRL_ALTERR,
    CTRL_GND,
    CTRL_TOFF,
    CTRL_FFALL,
    CTRL_BREAKOFF,
    CTRL_CNP
} ctrl_mode_t;

void ctrlInit();
void ctrlProcess();

ctrl_mode_t ctrlMode();
ctrl_mode_t ctrlModePrev();
float ctrlAlt();
void ctrlUpdate();

#endif // _ctrl_H
