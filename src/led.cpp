
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
    uint8_t off_fwd; 
    uint8_t off_bck;
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
        l.off_fwd = 0;
        l.off_bck = 0;
        l.beg = 0;
    }
}

void ledDisable() {
    for (auto &l : led) {
        digitalWrite(l.pin, LOW);
        //pinMode(l.pin, OUTPUT);
        l.ison = false;
        l.bit = NULL;
        l.off_fwd = 0;
        l.off_bck = 0;
        l.beg = 0;
    }
}

void ledSet(led_index_t li, const uint8_t *bit, uint8_t off_fwd, uint8_t off_bck, uint32_t tm) {
    auto &l = led[li];
    if (bit == NULL) {
        l.bit = NULL;
        l.off_fwd = 0;
        l.off_bck = 0;
        l.beg = 0;
    }
    else {
        l.bit = bit;
        l.off_fwd = off_fwd;
        l.off_bck = off_bck;
        l.beg = millis() - tm;
    }
}

void ledForce(led_index_t li, bool ison) {
    auto &l = led[li];
    l.ison = false;
    l.bit = NULL;
    l.off_fwd = 0;
    l.off_bck = 0;
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
        uint32_t m = (((millis() - l.beg) >> 6) + l.off_fwd) & 0x3f;     // делим millis() целочисленно на 64, а потом еще берём остаток от деления на 64
                                                                        // получается, что будем отсчитывать каждые 64 ms х 64 раз = 4096 ms (полный цикл)
        // l.off_fwd - сдвиг индекса времени на N позиций вперёд, поэтому его просто прибавляем до вычисления остатка от деления на 64
        // l.off_bck - это обратный отсчёт, начиная с N
        if (l.off_bck > 0)
            m = (l.off_bck - m) & 0x3f;
        
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
