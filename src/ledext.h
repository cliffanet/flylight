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
    LEDEXT_AUTO,
    LEDEXT_STAR,
    LEDEXT_CLOCKTST1,
    LEDEXT_CLOCKTST4,
    LEDEXT_CLOCKIN,
    LEDEXT_CLOCKOUT,
    LEDEXT_OUTMAX
} ledext_mode_t;

bool ledExtGet(ledext_mode_t &mode, uint32_t &tm);
void ledExtSet(ledext_mode_t _mode, uint32_t tm = 0);
void ledExtNext();
void ledExtDisconnect();

#endif // _ledext_H
