/*
    Menu Base class
*/

#ifndef _ledmon_H
#define _ledmon_H

#include "../def.h"

#define LEDMON_PIN      D3

typedef enum {
    LEDMON_NONE = 0,
    LEDMON_BLINK,
    LEDMON_ON,
    LEDMON_PWROFF,
    LEDMON_OK1,
    LEDMON_OK2,
    LEDMON_OK3,
    LEDMON_OK4,
    LEDMON_OK6,
    LEDMON_ERR2
} ledmon_sig_t;

void ledMonInit();
void ledMonForce(bool on);
void ledMonSet(int sig);
void ledMonProcess();

#endif // _ledmon_H
