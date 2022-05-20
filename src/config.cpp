// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "EmbUI.h"

void EmbUI::save(const char *_cfg, bool force){

    if ( sysData.fsDirty && !force ){
        LOG(println, F("UI: FS corrupt flag is set, won't write, u may try to reboot/reflash"));
        return;
    }
#ifdef ESP8266
    ESP.wdtDisable();
#endif
    File configFile;
    if (_cfg == nullptr) {
        LOG(println, F("UI: Save default main config file"));
        //LOG(printf_P, PSTR("%d, %d\n"),millis(),(sysData.asave * EMBUI_AUTOSAVE_MULTIPLIER * TASK_SECOND)+5000);
        if(millis()>(sysData.asave * EMBUI_AUTOSAVE_MULTIPLIER * TASK_SECOND)+5000)
            LittleFS.rename(FPSTR(P_cfgfile),FPSTR(P_cfgfile_bkp)); // unstable currently... sad, skipping first rename try
        configFile = LittleFS.open(FPSTR(P_cfgfile), "w"); // PSTR("w") использовать нельзя, будет исключение!
    } else {
        LOG(printf_P, PSTR("UI: Save %s main config file\n"), _cfg);
        configFile = LittleFS.open(_cfg, "w"); // PSTR("w") использовать нельзя, будет исключение!
    }
    serializeJson(cfg, configFile);
    configFile.close();
#ifdef ESP8266
    ESP.wdtEnable(WDTO_8S);
#endif
}

void EmbUI::load(const char *cfgfile){

    LOG(print, F("UI: Config file load "));

    if (cfgfile){
        if (loadjson(cfgfile, cfg))
            return;
    } else {
        String f = FPSTR(P_cfgfile);
        if (!loadjson(f.c_str(), cfg)){
            LOG(println, F("...failed, trying with backup"));
            f = FPSTR(P_cfgfile_bkp);
            if (loadjson(f.c_str(), cfg)){
                LOG(println, F("BackUp load OK!"));
                return;
            }
        } else {
            LOG(println, F("OK!"));
            return;
        }
    }

    // тут выясняется, что оба конфига повреждены, очищаем конфиг, он будет заполнен значениями по-умолчанию
    cfg.clear();
}

/**
 * tries to load json file from FS and deserialize it into provided DynamicJsonDocument
 */
bool EmbUI::loadjson(const char *filepath, DynamicJsonDocument &obj){
    if ( sysData.fsDirty ){
        LOG(println, F("UI: FS corrupt flag is set, won't load"));
        return false;
    }

    File jsonFile = LittleFS.open(filepath, "r");

    if (!jsonFile){
        LOG(printf_P, PSTR("Can't open file: %s"), filepath);
        return false;
    }

    DeserializationError error = deserializeJson(obj, jsonFile);
    jsonFile.close();

    if (!error)
        return true;
    
    LOG(printf_P, PSTR("UI: JSON deserializeJson error, file: %s, code: %d\n"), filepath, error.code());
    return false;
}