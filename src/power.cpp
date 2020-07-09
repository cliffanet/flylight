
#include "power.h"
#include "ledmon.h"
#include "button.h"

typedef struct {
    uint8_t hdr;
    uint8_t st;
} pwr_info_t;

static uint8_t rtcRead() {
    pwr_info_t inf;
    if (ESP.rtcUserMemoryRead(0, (uint32_t*) &inf, sizeof(inf))
        && (inf.hdr == '#') && ((inf.st == 0) || (inf.st == 1)))
        return inf.st;
    
    return 1;
}

static bool rtcSave(uint8_t v) {
    pwr_info_t inf = { '#', v };
    
    return ESP.rtcUserMemoryWrite(0, (uint32_t*) &inf, sizeof(inf));
}


static void hwOff() {
    ledMonDisable();
    
    // перед тем, как уйти в сон окончательно, дождёмся отпускания кнопки питания
    while (digitalRead(BUTTON_PIN) == LOW)
        delay(100);

    //Go to sleep now
    ESP.deepSleep(2000000, RF_DISABLED);
}

static void hwOn() {
}

/* ------------------------------------------------------------------------------------------- *
 *  
 * ------------------------------------------------------------------------------------------- */


bool pwrCheck() {
    bool ison = rtcRead() == 1;

    if (ison) {
        // если при загрузке обнаружили, что текущее состояние - "вкл", то включаемся
        hwOn();
        return true;
    }
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    int n = 0;
    while (digitalRead(BUTTON_PIN) == LOW) {
        if (n > 20) {
            // если кнопка нажата более 2 сек, 
            // сохраняем состояние как "вкл" и выходим с положительной проверкой
            if (!rtcSave(1))
                return false;
            hwOn();
            return true;
        }
        delay(100);
        n++;
    }

    hwOff();
    return false;
}

void pwrOff() {
    if (!rtcSave(0))
        return;
    
    hwOff();
}
