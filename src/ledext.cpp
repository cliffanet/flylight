
#include "ledext.h"
#include "led.h"
#if defined(MYNUM) && (MYNUM == 0)
#include "wifi.h"
#endif


/* ------------------------------------------------------------------------------------------- *
 *  мониторный светодиод (отображение режима и ошибок)
 * ------------------------------------------------------------------------------------------- */
static const ledarr_t led_solid = {
    0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111,
};

static const ledarr_t led_star1 = {
    0b11110000, 0b00001111, 0b00000000, 0b11110000, 0b00001111, 0b00000000, 0b11110000, 0b00000000
};
static const ledarr_t led_star2 = {
    0b00111100, 0b00000011, 0b11000000, 0b00111100, 0b00000011, 0b11000000, 0b00111100, 0b00000000
};

static ledext_mode_t mode = LEDEXT_NONE;
static uint32_t beg = 0;

bool ledExtGet(ledext_mode_t &_mode, uint32_t &tm) {
    _mode = mode;
    
    if (mode < LEDEXT_AUTO) {
        tm = 0;
        return false;
    }
    
    tm = millis() - beg;
    
    return mode >= LEDEXT_AUTO;
}

void ledExtSet(ledext_mode_t _mode, uint32_t tm) {
    mode = _mode;
    beg = millis()-tm;
#if defined(MYNUM) && (MYNUM == 0)
    wifiSendLight(mode);
#endif
    beg = millis();
    switch (mode) {
        case LEDEXT_NONE:
            ledSet(LED_EXT1, NULL);
            ledSet(LED_EXT2, NULL);
            ledSet(LED_EXT3, NULL);
            ledSet(LED_EXT4, NULL);
            return;
            
        case LEDEXT_AUTO:
            ledSet(LED_EXT1, led_solid);
            ledSet(LED_EXT2, led_solid);
            ledSet(LED_EXT3, led_solid);
            ledSet(LED_EXT4, led_solid);
            return;
        case LEDEXT_STAR:
            ledSet(LED_EXT1, led_star1, 0, 0, tm);
            ledSet(LED_EXT2, led_star2, 0, 0, tm);
            ledSet(LED_EXT3, led_star2, 0, 0, tm);
            ledSet(LED_EXT4, led_star1, 0, 0, tm);
            return;
        //case LEDEXT_CLOCKIN:    led = led_clock;    return;
        default:
            ledSet(LED_EXT1, NULL);
            ledSet(LED_EXT2, NULL);
            ledSet(LED_EXT3, NULL);
            ledSet(LED_EXT4, NULL);
            return;
    }
}
void ledExtNext() {
    if ((mode < LEDEXT_AUTO) || ((mode+1)>=LEDEXT_OUTMAX))
        ledExtSet(static_cast<ledext_mode_t>(LEDEXT_AUTO));
    else
        ledExtSet(static_cast<ledext_mode_t>(mode+1));
}

void ledExtDisconnect() {
    if (mode <= LEDEXT_AUTO)
        return;
    
    ledExtSet(LEDEXT_AUTO);
}
