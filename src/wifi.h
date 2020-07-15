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

#endif // _wifi_H
