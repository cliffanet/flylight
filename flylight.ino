
#include "def.h"

#include "src/ledmon.h"
#include "src/button.h"

//#include <WiFi.h>

//------------------------------------------------------------------------------
void btnClick() {
    static int sig = 0;
    
    sig ++;

    if (sig > 2) sig = 0;
    ledMonSet(sig);
}

void setup() {
    //if (!pwrCheck())
    //    return;
  
    //WiFi.mode(WIFI_OFF);

    // инициируем кнопки
    btnInit();
    btnHnd(BTN_SIMPLE, btnClick);
    
    ledMonInit();
}

//------------------------------------------------------------------------------
void loop() {
    static uint32_t m = millis();

    ledMonProcess();
    btnProcess();

    uint32_t m1 = millis();
    while (m <= m1) m+=64; // делаем так, чтобы цикл стартовал каждые 64ms
    delay(m - m1);
}
