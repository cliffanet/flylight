/*
    
*/

#ifndef _ledext_H
#define _ledext_H

#include "../def.h"

#define LEDEXT_PIN_1    LED_BUILTIN

typedef enum {
    LEDEXT_NONE = 0,
    LEDEXT_BLINK,
} ledext_mode_t;

void ledExtInit();
void ledExtDisable();
void ledExtSet(ledext_mode_t mode);
void ledExtNext();
void ledExtProcess();

#endif // _ledext_H
