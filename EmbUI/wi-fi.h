// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#pragma once

#define __EMBUI_WIFI_H

#ifdef ESP8266
 #include <ESP8266WiFi.h>
#endif

#ifdef ESP32
 #include <WiFi.h>
#endif

#define WIFI_CONNECT_TIMEOUT    5
#define WIFI_RECONNECT_TIMER    25
#define WIFI_PSK_MIN_LENGTH     8

//#endif