// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "EmbUI.h"

#ifdef ESP8266
 #define FORMAT_LITTLEFS_IF_FAILED
#endif

#ifdef ESP32
 #ifndef FORMAT_LITTLEFS_IF_FAILED
  #define FORMAT_LITTLEFS_IF_FAILED true
 #endif
#endif

void EmbUI::save(const char *_cfg, bool force){
    if ( sysData.cfgCorrupt && !force )
        return;

    LittleFS.begin();

    File configFile;
    if (_cfg == nullptr) {
        LOG(println, F("UI: Save default main config file"));
        LittleFS.rename(FPSTR(P_cfgfile),FPSTR(P_cfgfile_bkp));
        configFile = LittleFS.open(FPSTR(P_cfgfile), "w"); // PSTR("w") использовать нельзя, будет исключение!
    } else {
        LOG(printf_P, PSTR("UI: Save %s main config file\n"), _cfg);
        configFile = LittleFS.open(_cfg, "w"); // PSTR("w") использовать нельзя, будет исключение!
    }

    serializeJson(cfg, configFile);
}

void EmbUI::load(const char *_cfg){
    uint8_t retry_cnt = 0;

    while(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED) && retry_cnt<5){
        LOG(println, F("UI: Can't initialize LittleFS"));
        retry_cnt++;
        delay(100);
        //return;
    }

    if(retry_cnt==5 && !LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        LOG(println, F("UI: Fatal error - can't initialize LittleFS"));
        return;
    }
    
    File configFile = _cfg ? LittleFS.open(_cfg, "r") : LittleFS.open(FPSTR(P_cfgfile), "r");

    DeserializationError error;
    if (configFile){
        error = deserializeJson(cfg, configFile);
        configFile.close();
    } else {
        configFile = _cfg ? LittleFS.open(_cfg, "r") : LittleFS.open(FPSTR(P_cfgfile_bkp), "r"); // в случае ошибки пробуем восстановить конфиг из резервной копии
        if (configFile){
            error = deserializeJson(cfg, configFile);
            configFile.close();
        } else {
            LOG(println, F("UI: Fatal error - missed configs"));
            return;
        }
    }

    if (error) {
        configFile = _cfg ? LittleFS.open(_cfg, "r") : LittleFS.open(FPSTR(P_cfgfile_bkp), "r"); // в случае ошибки пробуем восстановить конфиг из резервной копии
        if (configFile){
            error = deserializeJson(cfg, configFile);
            configFile.close();
        }
        if(error){
            // тут выясняется, что оба конфига повреждены, запрещаем запись
#ifdef ESP8266
            LittleFS.check();
            LittleFS.gc();
#endif
            sysData.cfgCorrupt = true;
            LOG(print, F("UI: Critical JSON config deserializeJson error, config saving disabled: "));
            LOG(println, error.code());
        }
    }
}
