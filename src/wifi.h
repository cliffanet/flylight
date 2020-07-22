/*
    
*/

#ifndef _wifi_H
#define _wifi_H

#include "../def.h"

#define WIFI_CHANNEL            1 
#define WIFI_UPD_INTERVAL       10

typedef enum {
    WIFIST_NONE = 0,
    WIFIST_INITERR,
    WIFIST_INITOK,
    WIFIST_CONNECTERR,
    WIFIST_CONNECTOK,
    WIFIST_CONNECTALL
} wifi_state_t;

void wifiInit();
void wifiDisable();
wifi_state_t wifiState();
void wifiProcess();

#if defined(MYNUM) && (MYNUM == 0)
void wifiSendLight(uint8_t mode);
#endif

#endif // _wifi_H
