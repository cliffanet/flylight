
#include "def.h"

#include "src/power.h"
#include "src/ledmon.h"
#include "src/ledext.h"
#include "src/button.h"
#include "src/wifi.h"

//#include <WiFi.h>

//------------------------------------------------------------------------------
void setup() {
    if (!pwrCheck())
        return;
  
    //WiFi.mode(WIFI_OFF);
    
    // инициируем кнопки
    btnInit();
    btnHnd(BTN_SIMPLE, ledExtNext);
    btnHnd(BTN_LONG, pwrOffBegin);
}

//------------------------------------------------------------------------------
void loop() {
    static uint32_t m = millis();

    ledMonProcess();
    ledExtProcess();
    btnProcess();
    pwrOffProcess();
    wifiProcess();

    uint32_t m1 = millis();
    while (m <= m1) m+=64; // делаем так, чтобы цикл стартовал каждые 64ms
    delay(m - m1);
}
