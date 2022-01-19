// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#ifndef _EMBUI_GLOBALS_H
#define _EMBUI_GLOBALS_H

// Global macro's and framework libs
#include <Arduino.h>
#include "constants.h"

#if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
#include <umm_malloc/umm_malloc.h>
#include <umm_malloc/umm_heap_select.h>
#endif

// STRING Macro
#ifndef __STRINGIFY
 #define __STRINGIFY(a) #a
#endif
#define __I(x) x
#define TOSTRING(x) __STRINGIFY(x)

static const char PGversion[] PROGMEM = TOSTRING(EMBUI_VER);
#ifdef GIT_SRC_REV
static const char PGGITversion[] PROGMEM = GIT_SRC_REV;
#else
static const char PGGITversion[] PROGMEM = "";
#endif

// LOG macro's
#if defined(LOG)
  #undef LOG
#endif
#if defined(EMBUI_DEBUG)
  #ifndef EMBUI_DEBUG_PORT
    #define EMBUI_DEBUG_PORT Serial
  #endif
  #define LOG(func, ...) EMBUI_DEBUG_PORT.func(__VA_ARGS__)
#else
  #define LOG(func, ...) ;
#endif

#ifdef ESP32
 #include <functional>
#endif

typedef std::function<void(void)> callback_function_t;

#endif
