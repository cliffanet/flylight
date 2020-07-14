
#include "ledmon.h"


/* ------------------------------------------------------------------------------------------- *
 *  мониторный светодиод (отображение режима и ошибок)
 * ------------------------------------------------------------------------------------------- */

static const struct {
    //uint8_t    dev; // насколько надо сдвинуть значение /
    uint8_t     b[8]; // 64bit
} sigall[] = {
    { 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010 },
    { 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111 },
    { 0b00000000, 0b00000000, 0b00111000 },
    { 0b11100000 },
    { 0b11100011, 0b10000000 },
    { 0b11100011, 0b10001110 },
    { 0b11100011, 0b10001110, 0b00111000 },
    { 0b11100011, 0b10001110, 0b00111000, 0b11100011, 0b10000000 },
    { 0b11100000, 0b00001110, 0b00111000 },
};
static int sigcurr = 0;
static uint32_t sigbeg = 0;

void ledMonInit() {
    digitalWrite(LEDMON_PIN, LOW);
    pinMode(LEDMON_PIN, OUTPUT);
    sigcurr = 0;
}

void ledMonForce(bool on) {
    digitalWrite(LEDMON_PIN, on ? HIGH : LOW);
    //pinMode(LEDMON_PIN, INPUT);
}


void ledMonSet(int sig) {
    sigcurr = sig;
    sigbeg = millis();
}

void ledMonProcess() {
    static bool ison = true;
    
    if ((sigcurr < 1) || (sigcurr > (sizeof(sigall)/sizeof(sigall[0])))) {
        if (ison) {
            digitalWrite(LEDMON_PIN, LOW);
            ison = false;
        }
        return;
    }
    
    uint32_t m = ((millis() - sigbeg) >> 6) & 0x3f;     // делим millis() целочисленно на 64, а потом еще берём остаток от деления на 64
                                                        // получается, что будем отсчитывать каждые 64 ms х 64 раз = 4096 ms (полный цикл)
    auto &sig = sigall[sigcurr-1];                  // текущий сигнал
    uint8_t b = sig.b[m >> 3];                      // верхние 3 бита "индекса времени" = индекс элемента текущего сигнала (один из 8)
    bool val = (b >> 0x07 - (m & 0x07)) & 0x01 > 0; // нижние 3 бита "индекса времени" = номер бита, отсчитанного от самого старшего
    
    if (val != ison) {
        digitalWrite(LEDMON_PIN, val ? HIGH : LOW);
        ison = val;
    }
}
