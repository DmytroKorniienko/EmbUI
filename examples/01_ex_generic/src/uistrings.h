#pragma once

// Set of flash-strings that might be reused multiple times within the code

// General
static const char T_HEADLINE[] PROGMEM = "EmbUI Demo";    // имя проекта


// Our variable names
static const char V_LED[] PROGMEM = "vLED";
static const char V_VAR1[] PROGMEM = "v1";
static const char V_VAR2[] PROGMEM = "v2";

// UI blocks
static const char T_DEMO[] PROGMEM = "demo";     // генерация UI-секции "демо"
static const char T_MORE[] PROGMEM = "more";

// UI handlers
static const char T_SET_DEMO[] PROGMEM = "do_demo";     // обработка данных из секции "демо"
static const char T_SET_MORE[] PROGMEM = "do_more";
