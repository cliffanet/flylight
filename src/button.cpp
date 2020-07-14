
#include "button.h"
#include <vector>

/* ------------------------------------------------------------------------------------------- *
 * Инициализация кнопок
 * ------------------------------------------------------------------------------------------- */
static volatile btn_t btnst = { 0 };

static volatile button_hnd_t hndlast = NULL;
// this variable will be changed in the ISR (interrupt), and Read in main loop
// static: says this variable is only visible to function in this file, its value will persist, it is a global variable
// volatile: tells the compiler that this variables value must not be stored in a CPU register, it must exist
//   in memory at all times.  This means that every time the value of intTriggeredCount must be read or
//   changed, it has be read from memory, updated, stored back into RAM, that way, when the ISR 
//   is triggered, the current value is in RAM.  Otherwise, the compiler will find ways to increase efficiency
//   of access.  One of these methods is to store it in a CPU register, but if it does that,(keeps the current
//   value in a register, when the interrupt triggers, the Interrupt access the 'old' value stored in RAM, 
//   changes it, then returns to whatever part of the program it interrupted.  Because the foreground task,
//   (the one that was interrupted) has no idea the RAM value has changed, it uses the value it 'know' is 
//   correct (the one in the register).  

/* ------------------------------------------------------------------------------------------- *
 * Проверка состояния
 * ------------------------------------------------------------------------------------------- */
void IRAM_ATTR btnChkState() {
    uint8_t val = digitalRead(BUTTON_PIN);
    uint32_t m = millis();
    uint32_t tm = m-btnst.lastchg;
    
    if (tm >= BTN_FILTER_TIME) { // Защита от дребезга
        bool pushed = btnst.val == LOW; // Была ли нажата кнопка всё это время
        if (!pushed && (btnst.pushed != 0))  { // кнопка отпущена давно, можно сбросить флаги сработавших событий
            btnst.pushed = 0;
        }
        
        if (pushed) {
            if (((btnst.pushed & BTN_PUSHED_SIMPLE) == 0) && ((val != LOW) || (btnst.hndlong == NULL))) {
                btnst.pushed |= BTN_PUSHED_SIMPLE;
                hndlast = btnst.hndsmpl; // Простое нажатие
            }
            if (((btnst.pushed & BTN_PUSHED_LONG) == 0) && ((m-btnst.lastchg) >= BTN_LONG_TIME)) {
                btnst.pushed |= BTN_PUSHED_LONG | BTN_PUSHED_SIMPLE; // чтобы после длинного нажатия не сработало простое, помечаем простое тоже сработавшим
                hndlast = btnst.hndlong; // Длинное нажатие
            }
        }
    }
    
    if (btnst.val != val)
        btnst.lastchg = m; // время крайнего изменения состояния кнопки
    btnst.val = val;
}

/* ------------------------------------------------------------------------------------------- *
 * Инициализация - включаем пины и прописываем прерывания
 * ------------------------------------------------------------------------------------------- */
void btnInit() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    // Если поисле включения питания мы всё ещё держим кнопку,
    // дальше пока не работаем
    while (digitalRead(BUTTON_PIN) == LOW)
        delay(100);
    btnst.val = digitalRead(BUTTON_PIN);
    
    attachInterrupt(
        digitalPinToInterrupt(BUTTON_PIN),
        btnChkState,
        CHANGE
    );
}

/* ------------------------------------------------------------------------------------------- *
 * Периодическая обработка
 * ------------------------------------------------------------------------------------------- */
void btnProcess() {
    btnChkState();
    
    if (hndlast != NULL) {
        hndlast();
        hndlast = NULL;
    }
}

/* ------------------------------------------------------------------------------------------- *
 * Очистка всех назначенных обработчиков нажатий - нежно при смене режима
 * ------------------------------------------------------------------------------------------- */
void btnHndClear() {
    btnst.hndsmpl = NULL;
    btnst.hndlong = NULL;
}

/* ------------------------------------------------------------------------------------------- *
 * Установка одного обработчика нажатия на кнопку
 * ------------------------------------------------------------------------------------------- */
void btnHnd(button_time_t tm, button_hnd_t hnd) {
    switch (tm) {
        case BTN_SIMPLE:    btnst.hndsmpl = hnd; break;
        case BTN_LONG:      btnst.hndlong = hnd; break;
    }
}

bool btnPushed() {
    return digitalRead(BUTTON_PIN) == LOW;
}
