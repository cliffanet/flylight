
#include "led.h"
#include "ledmon.h"
#include "ledext.h"


/* ------------------------------------------------------------------------------------------- *
 *  работа со светодиодами (приводим всё к одному стилю)
 * ------------------------------------------------------------------------------------------- */

struct {
    uint8_t pin;
    bool    ison;
    const uint8_t *bit;
    uint8_t dly; 
    uint8_t mrr;
    uint32_t beg;
} led[5] = {
    { LEDMON_PIN },
    { LEDEXT_PIN_1 },
    { LEDEXT_PIN_2 },
    { LEDEXT_PIN_3 },
    { LEDEXT_PIN_4 },
};

void ledInit() {
    for (auto &l : led) {
        digitalWrite(l.pin, LOW);
        pinMode(l.pin, OUTPUT);
        l.ison = false;
        l.bit = NULL;
        l.dly = 0;
        l.mrr = 0;
        l.beg = 0;
    }
}

void ledDisable() {
    for (auto &l : led) {
        digitalWrite(l.pin, LOW);
        //pinMode(l.pin, OUTPUT);
        l.ison = false;
        l.bit = NULL;
        l.dly = 0;
        l.mrr = 0;
        l.beg = 0;
    }
}

void ledSet(led_index_t li, const uint8_t *bit, uint32_t tm, uint8_t dly, uint8_t mrr) {
    auto &l = led[li];
    if (bit == NULL) {
        l.bit = NULL;
        l.dly = 0;
        l.mrr = 0;
        l.beg = 0;
    }
    else {
        l.bit = bit;
        l.dly = dly;
        l.mrr = mrr;
        l.beg = millis() - tm;
    }
}

void ledForce(led_index_t li, bool ison) {
    auto &l = led[li];
    l.ison = false;
    l.bit = NULL;
    l.dly = 0;
    l.mrr = 0;
    l.beg = 0;
    digitalWrite(l.pin, ison ? HIGH : LOW);
}

void ledProcess() {
    uint32_t mill = millis();
    for (auto &l : led) {
        if (l.bit == NULL) {
            if (l.ison) {
                digitalWrite(l.pin, LOW);
                l.ison = false;
            }
            continue;
        }

        // m - индекс времени (0..63) - увеличивается на 1 каждые 64 мс
        uint32_t m = (((mill - l.beg) >> 6) - l.dly) & 0x3f;     // делим millis() целочисленно на 64, а потом еще берём остаток от деления на 64
                                                                        // получается, что будем отсчитывать каждые 64 ms х 64 раз = 4096 ms (полный цикл)
        // l.dly - задержка индекса времени на N позиций, поэтому его просто вычитаем до вычисления остатка от деления на 64
        // l.mrr - это обратный отсчёт, начиная с N
        // При использовании mrr надо не забыть сдвиг dly делать тоже обратным относительно общего числа устройств,
        // тут автоматизировать это не получится, это надо делать в ledExtSet с учётом алгоритма
        if (l.mrr > 0) {
            int8_t mrr = l.mrr;
            mrr -= m;
            m = static_cast<uint8_t>(mrr) & 0x3f;
        }
        
        // i - индекс массива = надо поделить индекс времени на 8
        uint8_t i = (m >> 3) & 0x7;
        // n - номер бита в 8-битном целом (в обратном порядке)
        uint8_t n = 7 - (m & 0x7);
        
        bool ison = ((l.bit[i] >> n) & 1) > 0;
        if (l.ison != ison) {
            l.ison = ison;
            digitalWrite(l.pin, ison ? HIGH : LOW);
        }
    }
}
