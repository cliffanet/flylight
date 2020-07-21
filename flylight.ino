
#include "def.h"

#include "src/power.h"
#include "src/led.h"
#include "src/button.h"
#include "src/wifi.h"
#include "src/ctrl.h"

//------------------------------------------------------------------------------
void setup() {
    if (!pwrCheck())
        return;
    
    // инициируем кнопки
    btnInit();

    ctrlInit();
}

//------------------------------------------------------------------------------
void loop() {
    static uint32_t m = millis();

    ctrlProcess();
    ledProcess();
    btnProcess();
    pwrOffProcess();
    wifiProcess();

    uint32_t m1 = millis();
    while (m <= m1) m+=64; // делаем так, чтобы цикл стартовал каждые 64ms
    delay(m - m1);
}
