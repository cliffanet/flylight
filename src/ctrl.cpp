
#include "ctrl.h"
#include "altcalc.h"
#include "../def.h"

#include "ledext.h"
#include "ledmon.h"
#include "button.h"
#include "power.h"
#include "wifi.h"

#include <Adafruit_BMP280.h>

static Adafruit_BMP280 bme; // hardware Wire

static altcalc ac;
static bool bmpok = false;
static ctrl_mode_t mode = CTRL_INIT;

/* ------------------------------------------------------------------------------------------- *
 * Базовые функции
 * ------------------------------------------------------------------------------------------- */



static void ctrlMonLed() {
    if (mode == CTRL_INIT) {
        ledMonSet(LEDMON_OK3);
        return;
    }
    
    switch (wifiState()) {
        case WIFIST_NONE:       ledMonSet(LEDMON_OK3);         return;
        case WIFIST_INITERR:    ledMonSet(ERR_WIFIINIT);        return;
        case WIFIST_INITOK:     ledMonSet(mode > CTRL_GND ? STATE_SKYOK       : STATE_WIFIOK);        return;
        case WIFIST_CONNECTERR: ledMonSet(ERR_WIFICONNECT);     return;
        case WIFIST_CONNECTOK:  ledMonSet(mode > CTRL_GND ? STATE_SKYCONNECT  : STATE_WIFICONNECT);   return;
        case WIFIST_CONNECTALL: ledMonSet(mode > CTRL_GND ? STATE_SKYALL      : STATE_WIFIALL);       return;
    }
}

static void ctrlAltMode(ctrl_mode_t _mode) {
    switch (_mode) {
        case CTRL_GND:
            btnHnd(BTN_SIMPLE, ledExtNextGnd);
            btnHnd(BTN_LONG, pwrOffBegin);
            break;
            
        case CTRL_TOFF:
            btnHnd(BTN_SIMPLE, ledExtNextTOff);
            btnHnd(BTN_LONG, NULL);
            break;
            
        case CTRL_FFALL:
            btnHnd(BTN_SIMPLE, ledExtNextFFall);
            btnHnd(BTN_LONG, NULL);
            break;
            
        case CTRL_BREAKOFF:
            btnHnd(BTN_SIMPLE, NULL);
            btnHnd(BTN_LONG, NULL);
            break;
            
        case CTRL_CNP:
            btnHnd(BTN_SIMPLE, NULL);
            btnHnd(BTN_LONG, NULL);
            break;
    }
    
    mode = _mode;
    
    ledExtSet(LEDEXT_AUTO);
    ctrlMonLed();
}


void ctrlInit() {
    bmpok = bme.begin(BMP280_ADDRESS_ALT);
}

void ctrlProcess() {
    if (!bmpok)
        return;
    
    ac.tick(bme.readPressure());
    
    // выясняем актуальный режим в зависимости от высоты
    ctrl_mode_t mode1 = mode;
    
    if ((ac.state() == ACST_INIT) || (ac.direct() == ACDIR_INIT))
        mode1 = CTRL_INIT;
    else
    if (ac.alt() < 40)
        mode1 = CTRL_GND;
    else if (ac.alt() < 800) {
        if ((mode == CTRL_GND) || ((ac.direct() == ACDIR_UP) && (ac.speed() > 1)))
            mode1 = CTRL_TOFF;
        else
        if ((ac.direct() == ACDIR_DOWN) && (ac.speed() > 4))
            mode1 = CTRL_CNP;
    }
    else
    if (ac.alt() < 1400) {
        if ((ac.direct() == ACDIR_DOWN) && (ac.speed() > 10))
            mode1 = CTRL_BREAKOFF;
    }
    else {
        if ((ac.direct() == ACDIR_DOWN) && (ac.speed() > 10))
            mode1 = CTRL_FFALL;
    }
    
    if (mode1 != mode)
        ctrlAltMode(mode1);
}


ctrl_mode_t ctrlMode() {
    return mode;
}

void ctrlUpdate() {
    ctrlAltMode(mode);
}
