#pragma once

// Set of flash-strings that might be reused multiple times within the code

// General
// General
#if defined CONFIG_IDF_TARGET_ESP32  
  static const char T_HEADLINE[] PROGMEM = "EmbUI ESP32 Demo";    // имя проекта
#elif defined CONFIG_IDF_TARGET_ESP32S3
  static const char T_HEADLINE[] PROGMEM = "EmbUI ESP32-S3 Demo";    // имя проекта
#elif defined CONFIG_IDF_TARGET_ESP32S2
  static const char T_HEADLINE[] PROGMEM = "EmbUI ESP32-S2 Demo";    // имя проекта
#elif defined CONFIG_IDF_TARGET_ESP32C3
  static const char T_HEADLINE[] PROGMEM = "EmbUI ESP32-C3 Demo";    // имя проекта
#else
  static const char T_HEADLINE[] PROGMEM = "EmbUI Demo";    // имя проекта
#endif

// Our variable names
static const char V_LED[] PROGMEM = "vLED";
static const char V_VAR1[] PROGMEM = "v1";
static const char V_VAR2[] PROGMEM = "v2";
static const char V_UPDRATE[] PROGMEM = "updrt";    // update rate

// UI blocks
static const char T_DEMO[] PROGMEM = "demo";     // генерация UI-секции "демо"

// UI handlers
static const char T_SET_DEMO[] PROGMEM = "do_demo";     // обработка данных из секции "демо"
static const char T_SET_MORE[] PROGMEM = "do_more";
