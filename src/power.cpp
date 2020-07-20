
#include "power.h"
#include "led.h"
#include "ledmon.h"
#include "ledext.h"
#include "wifi.h"
#include "button.h"

/* ------------------------------------------------------------------------------------------- *
 *  работа с rtc-памятью - сохраняем там текущее состояние вкл-выкл
 * ------------------------------------------------------------------------------------------- */
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

/* ------------------------------------------------------------------------------------------- *
 *  Работа с аппаратурой
 * ------------------------------------------------------------------------------------------- */


static void hwOff() {
    ledDisable();
    wifiDisable();
    
    // перед тем, как уйти в сон окончательно, дождёмся отпускания кнопки питания
    while (btnPushed())
        delay(100);
    
    if (!rtcSave(0))
        return;

    //Go to sleep now
    ESP.deepSleep(2000000, RF_DISABLED);
}

static void hwOn() {
    ledInit();
    wifiInit();
}

/* ------------------------------------------------------------------------------------------- *
 *  Проверка состояния при запуске
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
    while (btnPushed()) {
        if (n > 20) {
            // если кнопка нажата более 2 сек, 
            // сохраняем состояние как "вкл" и выходим с положительной проверкой
            if (!rtcSave(1))
                return false;
            hwOn();

            ledMonForce(true);
            while (btnPushed()) {};
            ledMonForce(false);
    
            return true;
        }
        delay(100);
        n++;
    }

    hwOff();
    return false;
}

/* ------------------------------------------------------------------------------------------- *
 *  Процедура отключения
 * ------------------------------------------------------------------------------------------- */
static uint8_t offstate = 0;
static uint32_t offtm = 0;
void pwrOffBegin() { // запуск отключения
    if (offstate > 0)
        return;
    
    offstate = 1;
    offtm = millis();
    ledExtSet(LEDEXT_NONE);
    ledForce(LED_MONITOR, true);
}

static void pwrOffFail() {
    ledMonSet(LEDMON_NONE); // запускаем сигнал ошибки
    offstate = 10;          // и дожидаемся его завершения
    offtm = millis();
    btnHndClear();
}

static void pwrOffClick() {
    uint32_t m = millis() - offtm;
    if ((m > 1400) && (m < 1800)) {
        offstate++;
        offtm = millis();
        
        if (offstate == 5) {
            btnHndClear();
            ledMonSet(LEDMON_OK6);
        }
        else {
            ledMonSet(LEDMON_PWROFF);
        }
    }
    else {
        pwrOffFail();
    }
}

void pwrOffProcess() {
    if (offstate == 0)
        return;
    
    switch (offstate) {
        case 1: // ожидаем отпускания кнопки
            if (digitalRead(BUTTON_PIN) == LOW) { // до сих пор нажата
                if ((offtm+4000) <= millis())
                    // кнопка зажата слишком долго, завершаем процесс
                    pwrOffFail();
            }
            else {
                offstate++;
                offtm = millis();
                btnHnd(BTN_SIMPLE, pwrOffClick);
                btnHnd(BTN_LONG, NULL);
                ledMonSet(LEDMON_PWROFF);
            }
            return;
        
        case 2:
        case 3:
        case 4: // клики согласно морганиям лампочки (тут нам надо проверить таймаут клика)
            if ((offtm+4000) <= millis())
                // кнопка зажата слишком долго, завершаем процесс
                pwrOffFail();
            return;
        
        case 5: // ожидаем завершения индикации лампочкой, чтобы окончательно выключить аппаратуру
            if ((offtm+2000) <= millis()) {
                // кнопка зажата слишком долго, завершаем процесс
                offstate = 0;
                offtm = 0;
                hwOff();
            }
            return;
        
        case 10: // ожидаем завершения индикации лампочкой, чтобы перейти в нормальный режим (ошибка при выключении)
            if ((offtm+4000) <= millis()) {
                // кнопка зажата слишком долго, завершаем процесс
                offstate = 0;
                offtm = 0;
                btnHnd(BTN_SIMPLE, ledExtNext);
                btnHnd(BTN_LONG, pwrOffBegin);
                // тут надо восстановить нужный режим моргания дежурного светодиода
                wifiModUpd();
            }
            return;
            
    }
}
