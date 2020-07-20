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
    LEDEXT_SOLIDALL,
    LEDEXT_BLINK,
    LEDEXT_SNAKE2,
    LEDEXT_CURT,
    LEDEXT_OUTMAX
} ledext_mode_t;

#define LEDEXT_AUTO     LEDEXT_SOLIDALL

void ledExtInit();
void ledExtDisable();
bool ledExtGet(ledext_mode_t &mode, uint32_t &tm);
void ledExtSync(ledext_mode_t mode, uint32_t tm);
void ledExtDisconnect();
void ledExtSet(ledext_mode_t mode);
void ledExtNext();
void ledExtProcess();

#endif // _ledext_H
