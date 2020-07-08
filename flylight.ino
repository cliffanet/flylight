
#include "def.h"

#include "src/ledmon.h"

//#include <WiFi.h>

//------------------------------------------------------------------------------
void setup() {
    //if (!pwrCheck())
    //    return;
  
    //WiFi.mode(WIFI_OFF);

    // инициируем кнопки
    //btnInit();
    
    ledMonInit();
}

//------------------------------------------------------------------------------
void loop() {
    static uint32_t m = millis();
    static uint32_t tm = 0;
    static int sig = 0;

    if (tm < millis()) {
        tm += 16000;
        sig ++;

        if (sig > 2) sig = 0;
        ledMonSet(sig);
    }

    ledMonProcess();

    uint32_t m1 = millis();
    while (m <= m1) m+=64; // делаем так, чтобы цикл стартовал каждые 64ms
    delay(m - m1);
}
