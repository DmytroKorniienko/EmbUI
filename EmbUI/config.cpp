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

    if ((sysData.isNeedSave || force)){
      LittleFS.begin();
    } else {
        return;
    }

    File configFile;
    if (_cfg == nullptr) {
        LOG(println, F("UI: Save default main config file"));
        configFile = LittleFS.open(FPSTR(P_cfgfile), "w"); // PSTR("w") использовать нельзя, будет исключение!
    } else {
        LOG(printf_P, PSTR("UI: Save %s main config file\n"), _cfg);
        configFile = LittleFS.open(_cfg, "w"); // PSTR("w") использовать нельзя, будет исключение!
    }

    String cfg_str;
    serializeJson(cfg, cfg_str);
    configFile.print(cfg_str);
    configFile.close();

    //cfg.garbageCollect(); // несколько раз ловил Exception (3) предположительно тут, возвращаю пока проверенный способ
    deserializeJson(cfg, cfg_str);
    sysData.isNeedSave = false;
    delay(DELAY_AFTER_FS_WRITING); // задержка после записи
}

void EmbUI::autosave(){
    if (sysData.isNeedSave && millis() > astimer + sysData.asave*1000){
        save();
        LOG(println, F("UI: AutoSave"));
        astimer = millis();
    }
}

void EmbUI::load(const char *_cfg){
    if (LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
        File configFile;
        if (_cfg == nullptr) {
            LOG(println, F("Load default main config file"));
            configFile = LittleFS.open(FPSTR(P_cfgfile), "r"); // PSTR("r") использовать нельзя, будет исключение!
        } else {
            LOG(printf_P, PSTR("Load %s main config file\n"), _cfg);
            configFile = LittleFS.open(_cfg, "r"); // PSTR("w") использовать нельзя, будет исключение!
        }

        String cfg_str = configFile.readString();
        configFile.close();
        if (cfg_str == F("")){
            LOG(println, F("Failed to open config file"));
            //save(); // this does nothing on a first run
            return;
        }
        DeserializationError error = deserializeJson(cfg, cfg_str);
        if (error) {
            LOG(print, F("JSON config deserializeJson error: "));
            LOG(println, error.code());
            return;
        }
    } else {
        LOG(println, F("UI: Can't initialize LittleFS"));
    }
}
