// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#ifndef _EMBUI_CONSTANTS_H
#define _EMBUI_CONSTANTS_H

// Interface
static const char P_block[] PROGMEM = "block";
static const char P_button[] PROGMEM = "button";
static const char P_color[] PROGMEM = "color";
static const char P_date[] PROGMEM = "date";
static const char P_directly[] PROGMEM = "directly";
static const char P_false[] PROGMEM = "0";
static const char P_file[] PROGMEM = "file";
static const char P_final[] PROGMEM = "final";
static const char P_hidden[] PROGMEM = "hidden";
static const char P_html[] PROGMEM = "html";
static const char P_id[] PROGMEM = "id";
static const char P_input[] PROGMEM = "input";
static const char P_label[] PROGMEM = "label";
static const char P_top_margine[] PROGMEM = "top_margine";
static const char P_editable[] PROGMEM = "editable";
static const char P_max[] PROGMEM = "max";
static const char P_menu[] PROGMEM = "menu";
static const char P_min[] PROGMEM = "min";
static const char P_null[] PROGMEM = "null";
static const char P_number[] PROGMEM = "number";
static const char P_options[] PROGMEM = "options";
static const char P_password[] PROGMEM = "password";
static const char P_pkg[] PROGMEM = "pkg";
static const char P_section[] PROGMEM = "section";
static const char P_step[] PROGMEM = "step";
static const char P_submit[] PROGMEM = "submit";
static const char P_time[] PROGMEM = "time";
static const char P_true[] PROGMEM = "1";
static const char P_type[] PROGMEM = "type";
static const char P_value[] PROGMEM = "value";
static const char P_wifi[] PROGMEM = "wifi";
static const char P_loading[] PROGMEM = "loading";
static const char P_confirm[] PROGMEM = "confirm";
static const char P_confirm_msg[] PROGMEM = "confirm_msg";

// UI colors
static const char P_RED[] PROGMEM = "red";
static const char P_ORANGE[] PROGMEM = "orange";
static const char P_YELLOW[] PROGMEM = "yellow";
static const char P_GREEN[] PROGMEM = "green";
static const char P_BLUE[] PROGMEM = "blue";
static const char P_GRAY[] PROGMEM = "gray";
static const char P_BLACK[] PROGMEM = "black";

// System configuration variables
static const char P_cfgfile[] PROGMEM = "/config.json";
static const char P_cfgfile_bkp[] PROGMEM = "/config_bkp.json";

static const char P_hostname[] PROGMEM = "hostname";    // System hostname
static const char P_WIFIMODE[] PROGMEM = "WiFiMode";    // WiFi mode (AP, STA, AP+STA)
static const char P_APpwd[] PROGMEM = "APpwd";          // AccessPoint password
static const char P_TZSET[] PROGMEM = "TZSET";          // TimeZone rule variable
static const char P_userntp[] PROGMEM = "userntp";      // user-defined NTP server
static const char P_DTIME[] PROGMEM = "datetime";
static const char P_LANGUAGE[] PROGMEM = "lang";        // UI language
static const char P_DEVICEDATETIME[] PROGMEM = "devicedatetime"; // hidden field for browser's date

// WiFi vars
static const char P_WCSSID[] PROGMEM = "wcssid";        // WiFi-Client SSID
static const char P_WCPASS[] PROGMEM = "wcpass";        // WiFi-Client password

// MQTT vars
static const char P_m_host[] PROGMEM = "m_host";
static const char P_m_pass[] PROGMEM = "m_pass";
static const char P_m_port[] PROGMEM = "m_port";
static const char P_m_pref[] PROGMEM = "m_pref";
static const char P_m_user[] PROGMEM = "m_user";
static const char P_m_tupd[] PROGMEM = "m_tupd";     // mqtt update interval

// http-related constants
static const char PGgzip[] PROGMEM = "gzip";
static const char PGhdrcachec[] PROGMEM = "Cache-Control";
static const char PGhdrcontentenc[] PROGMEM = "Content-Encoding";
static const char PGmimecss[] PROGMEM  = "text/css";
static const char PGmimehtml[] PROGMEM = "text/html; charset=utf-8";
static const char PGmimejson[] PROGMEM = "application/json";
static const char PGmimetxt[] PROGMEM  = "text/plain";
static const char PGmimexml[] PROGMEM  = "text/xml";
static const char PGnocache[] PROGMEM = "no-cache, no-store, must-revalidate";
static const char PG404[] PROGMEM  = "Not found";

// LOG Messages
static const char P_ERR_obj2large[] PROGMEM  = "UI: ERORR - can't add object to frame, too large!";

#ifdef EMBUI_USE_FTP
// FTP support
static const char P_ftpuser[] PROGMEM  = "ftp_user";
static const char P_ftppass[] PROGMEM  = "ftp_pass";
static const char P_FTP_USER_DEFAULT[] PROGMEM  = "ftp";
static const char P_FTP_PASS_DEFAULT[] PROGMEM  = "ftp";
#endif

// Bitmap for EmbUI
static const char P_cfgData[] PROGMEM  = "EmbUIData";

// UI blocks
static const char T_SETTINGS[] PROGMEM = "settings";
static const char T_OPT_NETW[] PROGMEM = "networking";
static const char T_LOAD_WIFI[] PROGMEM = "loading_wifi";

// UI handlers
static const char T_DO_OTAUPD[] PROGMEM = "update";
static const char T_UPROGRESS[] PROGMEM = "uprogress";
static const char T_SET_WIFI[] PROGMEM = "set_wifi";
static const char T_SET_WIFIAP[] PROGMEM = "set_wifiAP";
static const char T_SET_MQTT[] PROGMEM = "set_mqtt";
#ifdef EMBUI_USE_FTP
static const char T_SET_FTP[] PROGMEM = "set_ftp";
static const char T_CHK_FTP[] PROGMEM = "chk_ftp";
#endif
static const char T_SET_TIME[] PROGMEM = "set_time";
static const char T_SET_SCAN[] PROGMEM = "set_scan";

static const char T_SH_NETW[] PROGMEM = "sh_netw";
static const char T_SH_TIME[] PROGMEM = "sh_time";
static const char T_REBOOT[] PROGMEM = "reboot";

static const char T_XLOAD[] PROGMEM = "xload";

#endif // _EMBUI_CONSTANTS_H