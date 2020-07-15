
#include "wifi.h"
#include "ledmon.h"

#include <ESP8266WiFi.h>

#include <espnow.h>
#include "user_interface.h"

#ifndef ESP_OK
#define ESP_OK 0
#endif

static enum {
    WIFIST_NONE = 0,
    WIFIST_INITERR,
    WIFIST_INITOK,
    WIFIST_CONNECTERR,
    WIFIST_CONNECTOK
} state = WIFIST_NONE;

#if defined(MYNUM) && (MYNUM == 0)

static struct {
    bool connected;
    uint8_t mac[6];
    uint32_t last;
} peer[MAXNUM];

#elif defined(MYNUM)

static uint32_t beaclast  = 0;
 
#endif

static void wifirecv(uint8_t *mac, uint8_t *_data, uint8_t len) {
    if (state == WIFIST_CONNECTERR)
        return;
    const char *data = reinterpret_cast<const char*>(_data);
    //char ds[len];
    //memcpy(ds, data, len);
    //ds[len-1] = 0;
    //Serial.printf("Received Message[%02x:%02x:%02x:%02x:%02x:%02x]. Len=%d: %s\r\n",
    //              mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],
    //              len, ds);
                
    if ((len == 16) && (strncmp_P(data, PSTR("beacon-flylight"), 15) == 0)) {
    //    Serial.println("is beacon");
#if defined(MYNUM) && (MYNUM == 0)
        // Для головного получение такого пакета - это всегда ошибка дубликата мастера
        state = WIFIST_CONNECTERR;
        wifiModUpd();
#elif defined(MYNUM)
        // Для любого слейва сверяем мак головного
        uint8_t data[20];
        strcpy_P(reinterpret_cast<char*>(data), PSTR("slave-flylight"));
        data[15] = MYNUM;
        esp_now_send(mac, data, 16);
        if (state == WIFIST_INITOK) {
            state = WIFIST_CONNECTOK;
            wifiModUpd();
        } 
        beaclast = millis();
#endif
    }
    else
    if ((len == 16) && (strncmp_P(data, PSTR("slave-flylight"), 14) == 0)) {
#if defined(MYNUM) && (MYNUM == 0)
        // Для головного - обновление данных по пиру
        uint8_t num = _data[15];
        if ((num > 0) && (num <= MAXNUM)) {
            auto &p = peer[num-1];
            if (p.connected && (memcmp(p.mac, mac, sizeof(p.mac)) != 0)) {
                // mac пира изменился
                state = WIFIST_CONNECTERR;
                wifiModUpd();
            }
            else {
                if (!p.connected) {
                    p.connected = true;
                    memcpy(p.mac, mac, sizeof(p.mac));
                }
                p.last = millis();
            }
        }
#elif defined(MYNUM)
        // Для любого слейва получение такого пакета = ошибка
        state = WIFIST_CONNECTERR;
        wifiModUpd();
#endif
    }
}

#if defined(MYNUM) && (MYNUM == 0)
static void sndbcast_P(const char *datas) {
    uint8_t bcaddr[] = { 0xff,0xff,0xff,0xff,0xff,0xff };
    uint8_t data[20];
    strncpy_P(reinterpret_cast<char*>(data), datas, sizeof(data));
    
    esp_now_send(bcaddr, data, strlen(reinterpret_cast<char*>(data))+1);
}
#endif

void wifiInit() {
    //Serial.begin(115200);
#if defined(MYNUM) && (MYNUM == 0)
    bzero(peer, sizeof(peer));
#endif
    
    WiFi.softAP("flylight", "flylight123", WIFI_CHANNEL, false);
    WiFi.mode(WIFI_AP_STA);
    
    if ((esp_now_init() == ESP_OK) &&
        (esp_now_set_self_role(ESP_NOW_ROLE_COMBO) == ESP_OK) &&
        (esp_now_register_recv_cb(wifirecv) == ESP_OK)
        ) {
        state = WIFIST_INITOK;
    }
    else {
        state = WIFIST_INITERR;
    }
    
    wifiModUpd();
}

void wifiDisable() {
    esp_now_deinit();
    WiFi.mode(WIFI_OFF);
    state = WIFIST_NONE;
}

void wifiModUpd() {
    switch (state) {
        case WIFIST_NONE:       ledMonSet(LEDMON_NONE);         return;
        case WIFIST_INITERR:    ledMonSet(ERR_WIFIINIT);        return;
        case WIFIST_INITOK:     ledMonSet(STATE_WIFIOK);        return;
        case WIFIST_CONNECTERR: ledMonSet(ERR_WIFICONNECT);     return;
        case WIFIST_CONNECTOK:  ledMonSet(STATE_WIFICONNECT);   return;
    }
}


void wifiProcess() {
    if (state < WIFIST_INITOK)
        return;

#if defined(MYNUM) && (MYNUM == 0)
    // Отправка шиковещательного бикона
    static uint8_t nbc = 0;
    nbc++;
    if (nbc == WIFI_UPD_INTERVAL) {
        sndbcast_P(PSTR("beacon-flylight"));
        nbc = 0;
    }
    
    // Проверяем таймауты клиентов и все ли подсоединены
    uint8_t full = 0, n = 0;
    uint32_t timeout = millis() - (64*WIFI_UPD_INTERVAL*3);
    for (auto &p : peer) {
        n++;
        
        if (p.connected && (p.last <= timeout))
            p.connected = false;
        
        if ((n == (full+1)) && p.connected)
            full = n;
    }
    
    // Проверка текущего состояния
    if ((state == WIFIST_INITOK) || (state == WIFIST_CONNECTOK)) {
        auto st = full == MAXNUM ? WIFIST_CONNECTOK : WIFIST_INITOK;
        if (state != st) {
            state = st;
            wifiModUpd();
        }
    }
#elif defined(MYNUM)
    if ((state == WIFIST_INITOK) || (state == WIFIST_CONNECTOK)) {
        auto st = (beaclast+(64*WIFI_UPD_INTERVAL*3)) > millis() ? WIFIST_CONNECTOK : WIFIST_INITOK;
        if (state != st) {
            state = st;
            wifiModUpd();
        }
    }
#endif
}
