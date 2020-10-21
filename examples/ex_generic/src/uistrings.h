#pragma once

// Set of flash-strings that might be reused multiple times within the code

// General
static const char T_TRUE[] PROGMEM = "true";
static const char T_FALSE[] PROGMEM = "false";
static const char T_HEADLINE[] PROGMEM = "EmbUI Demo";    // имя проекта

// UI colors
static const char T_GRAY[] PROGMEM = "gray";

// UI blocks
static const char T_DEMO[] PROGMEM = "demo";
static const char T_MORE[] PROGMEM = "more";
static const char T_SETTINGS[] PROGMEM = "settings";
static const char T_OPT_NETW[] PROGMEM = "networking";

// UI handlers
static const char T_DO_OTAUPD[] PROGMEM = "otaupd";
static const char T_SET_WIFI[] PROGMEM = "set_wifi";
static const char T_SET_WIFIAP[] PROGMEM = "set_wifiAP";
static const char T_SET_MQTT[] PROGMEM = "set_mqtt";
static const char T_SET_TIME[] PROGMEM = "set_time";

static const char T_SH_NETW[] PROGMEM = "sh_netw";
static const char T_SH_TIME[] PROGMEM = "sh_time";
static const char T_SH_OTHER[] PROGMEM = "sh_other";

// WiFi vars
static const char T_HOSTNAME[] PROGMEM = "hostname";
static const char T_APONLY[] PROGMEM = "APonly";
static const char T_APPWD[] PROGMEM = "APpwd";
static const char T_WCSSID[] PROGMEM = "wcssid";
static const char T_WCPASS[] PROGMEM = "wcpass";

// MQTT vars
static const char T_MHOST[] PROGMEM = "m_host";
static const char T_MPORT[] PROGMEM = "m_port";
static const char T_MUSER[] PROGMEM = "m_user";
static const char T_MPASS[] PROGMEM = "m_pass";
static const char T_MPREF[] PROGMEM = "m_pref";
static const char T_MPERIOD[] PROGMEM = "mqtt_int";

// date/time/env vars
static const char T_TZSET[] PROGMEM = "TZSET";
static const char T_USERNTP[] PROGMEM = "userntp";
static const char T_DTIME[] PROGMEM = "datetime";
static const char T_LANGUAGE[] PROGMEM = "lang";
