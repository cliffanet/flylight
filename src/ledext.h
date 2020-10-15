/*
    
*/

#ifndef _ledext_H
#define _ledext_H

#include "../def.h"

#define LEDEXT_PIN_1    D5
#define LEDEXT_PIN_2    D6
#define LEDEXT_PIN_3    D7
#define LEDEXT_PIN_4    D8

typedef enum {
    LEDEXT_NONE = 0,
    LEDEXT_MINI,
    LEDEXT_EVOLUTION,
    LEDEXT_BREAKOFF,
    LEDEXT_FULL,
    LEDEXT_CNP,
    LEDEXT_STARCIRCLE,
    LEDEXT_STARLOOP,
    LEDEXT_CLOCKIN,
    LEDEXT_OUTMAX,
    LEDEXT_CLOCKTST1,
    LEDEXT_CLOCKTST4,
    LEDEXT_CLOCKOUT
} ledext_mode_t;

bool ledExtGet(ledext_mode_t &mode, uint32_t &tm);
void ledExtSet(ledext_mode_t _mode, uint32_t tm = 0);
#if defined(MYNUM) && (MYNUM > 0)
void ledExtRoot(ledext_mode_t _mode, uint32_t tm = 0);
#endif
void ledExtAltChg();
void ledExtNextGnd();
void ledExtNextTOff();
void ledExtNextFFall();
void ledExtDisconnect();

#endif // _ledext_H
