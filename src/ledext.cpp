
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
static const ledarr_t led_star3 = {
    0b11110000, 0b00000000, 0b00000000, 0b00000000, 0b11110000, 0b00000000, 0b00000000, 0b00000000
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
    
    if (mode <= LEDEXT_FULL) {
        tm = 0;
        return false;
    }
    
    tm = millis() - beg;
    
    return mode > LEDEXT_FULL;
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
            
        case LEDEXT_FULL:
        case LEDEXT_EVOLUTION:
        case LEDEXT_BREAKOFF:
            ledSet(LED_EXT1, led_solid);
            ledSet(LED_EXT2, led_solid);
            ledSet(LED_EXT3, led_solid);
            ledSet(LED_EXT4, led_solid);
            return;
        
        case LEDEXT_CNP:
            ledSet(LED_EXT1, led_solid);
            ledSet(LED_EXT2, led_cnp);
            ledSet(LED_EXT3, led_cnp);
            ledSet(LED_EXT4, led_solid);
            return;
        
        case LEDEXT_STARCIRCLE:
            ledSet(LED_EXT1, led_star1, tm);
            ledSet(LED_EXT2, led_star2, tm);
            ledSet(LED_EXT3, led_star2, tm);
            ledSet(LED_EXT4, led_star1, tm);
            return;

#if defined(MYNUM) && (MYNUM == 0)
#define SNUMTOP 16
#define SNUMBOT 0
#elif defined(MYNUM) && ((MYNUM == 1) || (MYNUM == 2))
#define SNUMTOP 18
#define SNUMBOT 2
#elif defined(MYNUM)
#define SNUMTOP 20
#define SNUMBOT 4
#endif
            
        case LEDEXT_STARLOOP:
            ledSet(LED_EXT1, led_star3, tm, SNUMTOP);
            ledSet(LED_EXT2, led_star3, tm, SNUMBOT);
            ledSet(LED_EXT3, led_star3, tm, SNUMBOT);
            ledSet(LED_EXT4, led_star3, tm, SNUMTOP);
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

void ledExtAltChg() {
    switch (ctrlMode()) {
        CTRL_INIT:
        CTRL_ALTERR:
            ledExtSet(LEDEXT_FULL);
            break;
            
        CTRL_GND:
            if (ctrlModePrev() == CTRL_INIT)
                ledExtSet(LEDEXT_NONE);
            break;
            
        CTRL_TOFF:
            ledExtSet(LEDEXT_NONE);
            break;
        
        CTRL_FFALL:
        CTRL_BREAKOFF:
            ledExtSet(LEDEXT_FULL);
            break;
        
        CTRL_CNP:
            ledExtSet(LEDEXT_CNP);
            break;
    }
}

void ledExtNextGnd() {
    switch (mode) {
        case LEDEXT_NONE:
            ledExtSet(LEDEXT_FULL);
            break;
        case LEDEXT_FULL:
            ledExtSet(LEDEXT_STARCIRCLE);
            break;
        case LEDEXT_STARCIRCLE:
            ledExtSet(LEDEXT_EVOLUTION);
            break;
        case LEDEXT_EVOLUTION:
            ledExtSet(LEDEXT_STARLOOP);
            break;
        case LEDEXT_STARLOOP:
            ledExtSet(LEDEXT_BREAKOFF);
            break;
        case LEDEXT_BREAKOFF:
            ledExtSet(LEDEXT_NONE);
            break;
    }
    
#if defined(MYNUM) && (MYNUM == 0)
    wifiSendLight(mode, millis() - beg);
#endif
}

void ledExtNextTOff() {
    ledExtSet(mode == LEDEXT_NONE ? LEDEXT_MINI : LEDEXT_NONE);
}

void ledExtNextFFall() {
    switch (mode) {
        case LEDEXT_FULL:
            ledExtSet(LEDEXT_STARCIRCLE);
            break;
        case LEDEXT_STARCIRCLE:
            ledExtSet(LEDEXT_EVOLUTION);
            break;
        case LEDEXT_EVOLUTION:
            ledExtSet(LEDEXT_STARLOOP);
            break;
        case LEDEXT_STARLOOP:
            ledExtSet(LEDEXT_FULL);
            break;
    }

#if defined(MYNUM) && (MYNUM == 0)
    wifiSendLight(mode, millis() - beg);
#endif
}

void ledExtConnect(ledext_mode_t _mode, uint32_t tm) {
    switch (ctrlMode()) {
        CTRL_INIT:
        CTRL_ALTERR:
        CTRL_TOFF:
        CTRL_BREAKOFF:
        CTRL_CNP:
            return;
    }
    ledExtSet(_mode, tm);
}

void ledExtDisconnect() {
    ledExtSet(LEDEXT_FULL);
}
