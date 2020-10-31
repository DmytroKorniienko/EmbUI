#pragma once

// Set of flash-strings that might be reused multiple times within the code

// General
static const char T_HEADLINE[] PROGMEM = "EmbUI Demo";    // имя проекта

// UI colors
static const char T_GRAY[] PROGMEM = "gray";

// UI blocks
static const char T_DEMO[] PROGMEM = "demo";
static const char T_MORE[] PROGMEM = "more";
static const char T_SETTINGS[] PROGMEM = "settings";
static const char T_OPT_NETW[] PROGMEM = "networking";

// UI handlers
static const char T_DO_OTAUPD[] PROGMEM = "update";
static const char T_SET_WIFI[] PROGMEM = "set_wifi";
static const char T_SET_WIFIAP[] PROGMEM = "set_wifiAP";
static const char T_SET_MQTT[] PROGMEM = "set_mqtt";
static const char T_SET_TIME[] PROGMEM = "set_time";

static const char T_SH_NETW[] PROGMEM = "sh_netw";
static const char T_SH_TIME[] PROGMEM = "sh_time";
static const char T_SH_OTHER[] PROGMEM = "sh_other";

// WiFi vars
static const char T_WCSSID[] PROGMEM = "wcssid";
static const char T_WCPASS[] PROGMEM = "wcpass";

// MQTT vars
static const char T_MPERIOD[] PROGMEM = "mqtt_int";

// date/time/env vars
static const char T_DTIME[] PROGMEM = "datetime";
static const char T_LANGUAGE[] PROGMEM = "lang";
