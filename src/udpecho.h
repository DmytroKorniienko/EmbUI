// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#ifndef _EMBUI_UDPECHO_H
#define _EMBUI_UDPECHO_H

#ifdef EMBUI_USE_UDP

#ifdef ESP32
#include <AsyncUDP.h>
#else
#include <ESPAsyncUDP.h>
#endif

#ifndef EMBUI_UDP_PORT
#define EMBUI_UDP_PORT            5560    // UDP server port
#endif

#endif

#endif