
#include "def.h"

#include "src/power.h"
#include "src/ledmon.h"
#include "src/button.h"

//#include <WiFi.h>

//------------------------------------------------------------------------------
void sigupd(bool _inc = true) {
    static int sig = 0;

    if (_inc) {
      sig ++;
      if (sig > 3) sig = 0;
    }
    
    ledMonSet(LEDMON_OK3);
}
    
void btnClick() {
    sigupd();
}

void setup() {
    if (!pwrCheck())
        return;
  
    //WiFi.mode(WIFI_OFF);
    
    ledMonInit();

    // инициируем кнопки
    btnInit();
    btnHnd(BTN_SIMPLE, btnClick);
    btnHnd(BTN_LONG, pwrOffBegin);
}

//------------------------------------------------------------------------------
void loop() {
    static uint32_t m = millis();

    ledMonProcess();
    btnProcess();
    pwrOffProcess();

    uint32_t m1 = millis();
    while (m <= m1) m+=64; // делаем так, чтобы цикл стартовал каждые 64ms
    delay(m - m1);
}
