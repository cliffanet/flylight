
#include "ledext.h"


/* ------------------------------------------------------------------------------------------- *
 *  мониторный светодиод (отображение режима и ошибок)
 * ------------------------------------------------------------------------------------------- */

static const uint8_t *led = NULL;

static const uint8_t led_blink[32] = {
    0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010,
    0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,
    0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010, 0b10101010,
    0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,
};
static const uint8_t led_snake2[32] = {
    0b10001000, 0b11001100, 0b01100110, 0b00110011, 0b00010001, 0b00000000, 0b00000000, 0b00000000,
    0b10001000, 0b11001100, 0b01100110, 0b00110011, 0b00010001, 0b00000000, 0b00000000, 0b00000000,
    0b10001000, 0b11001100, 0b01100110, 0b00110011, 0b00010001, 0b00000000, 0b00000000, 0b00000000,
    0b10001000, 0b11001100, 0b01100110, 0b00110011, 0b00010001, 0b00000000, 0b00000000, 0b00000000,
};
static const uint8_t led_curt[32] = {
    0b01100110, 0b10011001, 0b00000000, 0b10011001, 0b01100110, 0b00000000, 0b00000000, 0b00000000,
    0b01100110, 0b10011001, 0b00000000, 0b10011001, 0b01100110, 0b00000000, 0b00000000, 0b00000000,
    0b01100110, 0b10011001, 0b00000000, 0b10011001, 0b01100110, 0b00000000, 0b00000000, 0b00000000,
    0b01100110, 0b10011001, 0b00000000, 0b10011001, 0b01100110, 0b00000000, 0b00000000, 0b00000000,
};

static uint32_t beg = 0;
static uint8_t ison = 0;

void ledExtInit() {
    digitalWrite(LEDEXT_PIN_1, LOW);
    pinMode(LEDEXT_PIN_1, OUTPUT);
    digitalWrite(LEDEXT_PIN_2, LOW);
    pinMode(LEDEXT_PIN_2, OUTPUT);
    digitalWrite(LEDEXT_PIN_3, LOW);
    pinMode(LEDEXT_PIN_3, OUTPUT);
    digitalWrite(LEDEXT_PIN_4, LOW);
    pinMode(LEDEXT_PIN_4, OUTPUT);
    led = NULL;
    beg = 0;
}

void ledExtDisable() {
    digitalWrite(LEDEXT_PIN_1, LOW);
    //pinMode(LEDEXT_PIN_1, INPUT);
    digitalWrite(LEDEXT_PIN_2, LOW);
    //pinMode(LEDEXT_PIN_2, INPUT);
    digitalWrite(LEDEXT_PIN_3, LOW);
    //pinMode(LEDEXT_PIN_3, INPUT);
    digitalWrite(LEDEXT_PIN_4, LOW);
    //pinMode(LEDEXT_PIN_4, INPUT);
}


void ledExtSet(ledext_mode_t mode) {
    beg = millis();
    switch (mode) {
        case LEDEXT_NONE:       led = NULL;         return;
        case LEDEXT_BLINK:      led = led_blink;    return;
        case LEDEXT_SNAKE2:     led = led_snake2;   return;
        case LEDEXT_CURT:       led = led_curt;     return;
        default:                led = NULL;
    }
}
void ledExtNext() {
    if (led == NULL)
        ledExtSet(LEDEXT_BLINK);
    else
    if (led == led_blink)
        ledExtSet(LEDEXT_SNAKE2);
    else
    if (led == led_snake2)
        ledExtSet(LEDEXT_CURT);
    else
    if (led == led_curt)
        ledExtSet(LEDEXT_NONE);
    else
        ledExtSet(LEDEXT_NONE);
}

static void setstate(uint8_t st) {
    if (st == ison)
        return;
    
    if ((st & 0x8) != (ison & 0x8))
        digitalWrite(LEDEXT_PIN_1, (st & 0x8) > 0 ? HIGH : LOW);
    if ((st & 0x4) != (ison & 0x4))
        digitalWrite(LEDEXT_PIN_2, (st & 0x4) > 0 ? HIGH : LOW);
    if ((st & 0x2) != (ison & 0x2))
        digitalWrite(LEDEXT_PIN_3, (st & 0x2) > 0 ? HIGH : LOW);
    if ((st & 0x1) != (ison & 0x1))
        digitalWrite(LEDEXT_PIN_4, (st & 0x1) > 0 ? HIGH : LOW);
    
    ison = st;
}

void ledExtProcess() {
    static bool ison = true;
    
    if (led == NULL) {
        if (ison > 0)
            setstate(0);
        return;
    }
    
    uint32_t m = ((millis() - beg) >> 6) & 0x3f;     // делим millis() целочисленно на 64, а потом еще берём остаток от деления на 64
                                                        // получается, что будем отсчитывать каждые 64 ms х 64 раз = 4096 ms (полный цикл)
    auto st = led[m >> 1];    // каждый байт в массиве - это два блока по 4 бит
    if ((m & 0x1) == 0)         // первым используем старший блок, потом младший
        st = st >> 4;
    else
        st = st & 0b1111;
    
    setstate(st);
}
