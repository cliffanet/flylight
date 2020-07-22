
#include "ledext.h"
#include "led.h"
#include "ctrl.h"
#if defined(MYNUM) && (MYNUM == 0)
#include "wifi.h"
#endif


/* ------------------------------------------------------------------------------------------- *
 *  мониторный светодиод (отображение режима и ошибок)
 * ------------------------------------------------------------------------------------------- */
static const ledarr_t led_solid = {
    0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111,
};

static const ledarr_t led_cnp = {
    0b11111111, 0b00001111, 0b00001111, 0b11111111, 0b11111111, 0b00001111, 0b00001111, 0b11111111,
};

static const ledarr_t led_star1 = {
    0b11110000, 0b00001111, 0b00000000, 0b11110000, 0b00001111, 0b00000000, 0b11110000, 0b00000000
};
static const ledarr_t led_star2 = {
    0b00111100, 0b00000011, 0b11000000, 0b00111100, 0b00000011, 0b11000000, 0b00111100, 0b00000000
};

static const ledarr_t led_clock3 = {
    //0b11110000, 0b00001111, 0b00000000, 0b11110000, 0b00001111, 0b00000000, 0b11110000, 0b00000000
    0b11111111, 0b00000000, 0b00000000, 0b11111111, 0b00000000, 0b00000000, 0b00000000, 0b00000000
};

static const ledarr_t led_clock6 = {
    //0b11110000, 0b00001111, 0b00000000, 0b11110000, 0b00001111, 0b00000000, 0b11110000, 0b00000000
    0b11110000, 0b00000000, 0b00000000, 0b11110000, 0b00000000, 0b00000000, 0b00000000, 0b00000000
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
    switch (mode) {
        case LEDEXT_NONE:
            ledSet(LED_EXT1, NULL);
            ledSet(LED_EXT2, NULL);
            ledSet(LED_EXT3, NULL);
            ledSet(LED_EXT4, NULL);
            return;
            
        case LEDEXT_MINI:
            ledSet(LED_EXT1, NULL);
            ledSet(LED_EXT2, led_solid);
            ledSet(LED_EXT3, NULL);
            ledSet(LED_EXT4, NULL);
            return;
            
        case LEDEXT_AUTO:
            switch (ctrlMode()) {
                case CTRL_INIT:
                case CTRL_TOFF:
                    ledSet(LED_EXT1, NULL);
                    ledSet(LED_EXT2, NULL);
                    ledSet(LED_EXT3, NULL);
                    ledSet(LED_EXT4, NULL);
                    return;
                
                case CTRL_GND:
                case CTRL_FFALL:
                case CTRL_BREAKOFF:
                    ledSet(LED_EXT1, led_solid);
                    ledSet(LED_EXT2, led_solid);
                    ledSet(LED_EXT3, led_solid);
                    ledSet(LED_EXT4, led_solid);
                    return;
                    
                case CTRL_CNP:
                    ledSet(LED_EXT1, led_solid);
                    ledSet(LED_EXT2, led_cnp);
                    ledSet(LED_EXT3, led_cnp);
                    ledSet(LED_EXT4, led_solid);
                    return;
            }
            
        case LEDEXT_STAR:
            ledSet(LED_EXT1, led_star1, tm);
            ledSet(LED_EXT2, led_star2, tm);
            ledSet(LED_EXT3, led_star2, tm);
            ledSet(LED_EXT4, led_star1, tm);
            return;

#if defined(MYNUM) && (MYNUM == 0)
#define REVNUM 0
#elif defined(MYNUM)
#define REVNUM (MAXNUM+1-MYNUM)
#endif
            
        case LEDEXT_CLOCKTST1:
            ledSet(LED_EXT1, led_clock3, tm, MYNUM*8);
            ledSet(LED_EXT2, NULL);
            ledSet(LED_EXT3, NULL);
            ledSet(LED_EXT4, NULL);
            return;
        case LEDEXT_CLOCKTST4:
            ledSet(LED_EXT1, NULL);
            ledSet(LED_EXT2, NULL);
            ledSet(LED_EXT3, NULL);
            ledSet(LED_EXT4, led_clock3, tm, REVNUM*8, 31);
            return;
            
        case LEDEXT_CLOCKIN:
            ledSet(LED_EXT1, led_clock3, tm, MYNUM*8);
            ledSet(LED_EXT2, led_solid, tm);
            ledSet(LED_EXT3, led_solid, tm);
            ledSet(LED_EXT4, led_clock3, tm, REVNUM*8, 31);
            return;
            
        case LEDEXT_CLOCKOUT:
            ledSet(LED_EXT1, NULL);
            ledSet(LED_EXT2, led_clock6, tm, MYNUM*8+4);
            ledSet(LED_EXT3, led_clock6, tm, MYNUM*8);
            ledSet(LED_EXT4, NULL);
            return;
        
        default:
            ledSet(LED_EXT1, NULL);
            ledSet(LED_EXT2, NULL);
            ledSet(LED_EXT3, NULL);
            ledSet(LED_EXT4, NULL);
            return;
    }
}

void ledExtNextGnd() {
    if (mode == LEDEXT_NONE)
        ledExtSet(LEDEXT_AUTO);
    else
    if ((mode < LEDEXT_AUTO) || ((mode+1)>=LEDEXT_OUTMAX))
        ledExtSet(LEDEXT_NONE);
    else
        ledExtSet(static_cast<ledext_mode_t>(mode+1));
    
#if defined(MYNUM) && (MYNUM == 0)
    wifiSendLight(mode);
#endif
}

void ledExtNextTOff() {
    ledExtSet(mode == LEDEXT_AUTO ? LEDEXT_MINI : LEDEXT_AUTO);
}

void ledExtNextFFall() {
    if ((mode <= LEDEXT_AUTO) || ((mode+1)>=LEDEXT_OUTMAX))
        ledExtSet(static_cast<ledext_mode_t>(LEDEXT_AUTO+1));
    else
        ledExtSet(static_cast<ledext_mode_t>(mode+1));

#if defined(MYNUM) && (MYNUM == 0)
    wifiSendLight(mode);
#endif
}

void ledExtDisconnect() {
    if (mode <= LEDEXT_AUTO)
        return;
    
    ledExtSet(LEDEXT_AUTO);
}
