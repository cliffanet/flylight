
#include "ctrl.h"
#include "altcalc.h"
#include "../def.h"

#include "ledext.h"
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

static void ctrlUpdate(ctrl_mode_t _mode);

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
        ctrlUpdate(mode1);
}


ctrl_mode_t ctrlMode() {
    return mode;
}

static void ctrlUpdate(ctrl_mode_t _mode) {
    switch (_mode) {
        case CTRL_GND:
            btnHnd(BTN_SIMPLE, ledExtNextGnd);
            btnHnd(BTN_LONG, pwrOffBegin);
            wifiModUpd();
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
    
    ledExtSet(LEDEXT_AUTO);
    wifiModUpd();
    
    mode = _mode;
}

void ctrlUpdate() {
    ctrlUpdate(mode);
}
