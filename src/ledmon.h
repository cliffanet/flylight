/*
    Menu Base class
*/

#ifndef _ledmon_H
#define _ledmon_H

#include "../def.h"

#define LEDMON_PIN      D3

typedef enum {
    LEDMON_NOCHG = -1,
    LEDMON_NONE = 0,
    LEDMON_BLINK,
    LEDMON_ON,
    LEDMON_PWROFF,
    LEDMON_OK1,
    LEDMON_OK2,
    LEDMON_OK3,
    LEDMON_OK4,
    LEDMON_OK6,
    LEDMON_ERR2,
    LEDMON_ERR3,
    LEDMON_ERR4
} ledmon_sig_t;

#define STATE_WIFIOK        LEDMON_OK1
#define STATE_WIFICONNECT   LEDMON_OK2
#define STATE_WIFIALL       LEDMON_OK3
#define ERR_WIFIINIT        LEDMON_ERR3
#define ERR_WIFICONNECT     LEDMON_ERR4

void ledMonInit();
void ledMonForce(bool on);
void ledMonSet(int sig);
void ledMonProcess();

#endif // _ledmon_H
