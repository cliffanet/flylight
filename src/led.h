/*
    Menu Base class
*/

#ifndef _led_H
#define _led_H

#include "../def.h"

#define LEDMON_PIN      D3

typedef enum {
    LED_MONITOR = 0,
    LED_EXT1,
    LED_EXT2,
    LED_EXT3,
    LED_EXT4,
} led_index_t;

typedef uint8_t ledarr_t[8];

void ledInit();
void ledDisable();
void ledSet(led_index_t li, const uint8_t *bit, uint8_t dly = 0, uint8_t mrr = 0, uint32_t tm = 0);
void ledForce(led_index_t li, bool ison);
void ledProcess();

#endif // _led_H
