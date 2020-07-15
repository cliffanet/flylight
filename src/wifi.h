/*
    
*/

#ifndef _wifi_H
#define _wifi_H

#include "../def.h"

#define WIFI_CHANNEL            1 
#define WIFI_UPD_INTERVAL       10

void wifiInit();
void wifiDisable();
void wifiModUpd();
void wifiProcess();

#if defined(MYNUM) && (MYNUM == 0)
void wifiSendLight(uint8_t mode);
#endif

#endif // _wifi_H
