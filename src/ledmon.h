/*
    Menu Base class
*/

#ifndef _ledmon_H
#define _ledmon_H

#include "../def.h"

#define LEDMON_PIN      LED_BUILTIN

void ledMonInit();
void ledMonSet(int sig);
void ledMonProcess();

#endif // _ledmon_H
