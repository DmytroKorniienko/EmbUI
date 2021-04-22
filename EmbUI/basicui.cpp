#include "basicui.h"

uint8_t lang;            // default language for text resources


/**
 * Define configuration variables and controls handlers
 * 
 * Variables has literal names and are kept within json-configuration file on flash
 * Control handlers are bound by literal name with a particular method. This method is invoked
 * by manipulating controls
 * 
 * this method owerrides weak definition in framework
 * 
 */
void BasicUI::add_sections(){
    LOG(println, F("UI: Creating webui vars"));

    // variable for UI language (specific to basicui translations)
    embui.var_create(FPSTR(P_LANGUAGE), LANG::RU);

    lang = embui.param(FPSTR(P_LANGUAGE)).toInt();
    /**
     * обработчики действий
     */ 
    // вывод BasicUI секций
    embui.section_handle_add(FPSTR(T_SETTINGS), section_settings_frame);    // generate "settings" UI section
    embui.section_handle_add(FPSTR(T_SH_NETW), block_settings_netw);        // generate "network settings" UI section
    embui.section_handle_add(FPSTR(T_SH_TIME), block_settings_time);         // generate "time settings" UI section
    //embui.section_handle_add(FPSTR(T_SH_OTHER), show_settings_other);

    // обработка базовых настроек
    embui.section_handle_add(FPSTR(T_SET_WIFI), set_settings_wifi);         // обработка настроек WiFi Client
    embui.section_handle_add(FPSTR(T_SET_WIFIAP), set_settings_wifiAP);     // обработка настроек WiFi AP
    embui.section_handle_add(FPSTR(T_SET_MQTT), set_settings_mqtt);         // обработка настроек MQTT
    embui.section_handle_add(FPSTR(T_SET_TIME), set_settings_time);         // установки даты/времени
    embui.section_handle_add(FPSTR(P_LANGUAGE), set_language);              // смена языка интерфейса
    embui.section_handle_add(FPSTR(T_REBOOT), set_reboot);              // смена языка интерфейса

    //embui.section_handle_add(FPSTR(T_004B), set_settings_other);
}

/**
 * This code adds "Settings" section to the MENU
 * it is up to you to properly open/close Interface menu json_section
 */
void BasicUI::opt_setup(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->option(FPSTR(T_SETTINGS), FPSTR(T_DICT[lang][TD::D_SETTINGS]));     // пункт меню "настройки"
}

/**
 * формирование секции "настроек",
 * вызывается либо по выбору из "меню" либо при вызове из
 * других блоков/обработчиков
 * 
 */
void BasicUI::section_settings_frame(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->json_frame_interface("");       // саму секцию целиком не обрабатываем

    interf->json_section_main(FPSTR(T_SETTINGS), FPSTR(T_DICT[lang][TD::D_SETTINGS]));

    interf->select(FPSTR(P_LANGUAGE), String(lang), String(FPSTR(T_DICT[lang][TD::D_LANG])), true);
    interf->option("0", F("Rus"));
    interf->option("1", F("Eng"));
    interf->json_section_end();

    interf->spacer();

    interf->button(FPSTR(T_SH_NETW), FPSTR(T_DICT[lang][TD::D_WIFI_MQTT]));  // кнопка перехода в настройки сети
    interf->button(FPSTR(T_SH_TIME), FPSTR(T_DICT[lang][TD::D_Time]));       // кнопка перехода в настройки времени

    // call for user_defined function that may add more elements to the "settings page"
    user_settings_frame(interf, data);

    interf->spacer();
    block_settings_update(interf, data);                                     // добавляем блок интерфейса "обновления ПО"

    interf->json_section_end();
    interf->json_frame_flush();
}

/**
 *  BasicUI блок интерфейса настроек WiFi/MQTT
 */
void BasicUI::block_settings_netw(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->json_frame_interface();

    // Headline
    interf->json_section_main(FPSTR(T_OPT_NETW), FPSTR(T_DICT[lang][TD::D_WIFI_MQTT]));

    // форма настроек Wi-Fi Client
    interf->json_section_hidden(FPSTR(T_SET_WIFI), FPSTR(T_DICT[lang][TD::D_WiFiClient]));
    interf->spacer(FPSTR(T_DICT[lang][TD::D_WiFiClientOpts]));
    interf->text(FPSTR(P_hostname), FPSTR(T_DICT[lang][TD::D_Hostname]));
    interf->text(FPSTR(P_WCSSID), WiFi.SSID(), FPSTR(T_DICT[lang][TD::D_WiFiSSID]), false);
    interf->password(FPSTR(P_WCPASS), "", FPSTR(T_DICT[lang][TD::D_Password]));
    interf->button_submit(FPSTR(T_SET_WIFI), FPSTR(T_DICT[lang][TD::D_CONNECT]), FPSTR(P_GRAY));
    interf->json_section_end();

    // форма настроек Wi-Fi AP
    interf->json_section_hidden(FPSTR(T_SET_WIFIAP), FPSTR(T_DICT[lang][TD::D_WiFiAP]));
    interf->text(FPSTR(P_hostname), FPSTR(T_DICT[lang][TD::D_Hostname]));
    interf->spacer(FPSTR(T_DICT[lang][TD::D_WiFiAPOpts]));
    interf->comment(FPSTR(T_DICT[lang][TD::D_MSG_APOnly]));
    interf->checkbox(FPSTR(P_APonly), FPSTR(T_DICT[lang][TD::D_APOnlyMode]));
    interf->password(FPSTR(P_APpwd),  FPSTR(T_DICT[lang][TD::D_MSG_APProtect]));
    interf->button_submit(FPSTR(T_SET_WIFIAP), FPSTR(T_DICT[lang][TD::D_SAVE]), FPSTR(P_GRAY));
    interf->json_section_end();

    // форма настроек MQTT
    interf->json_section_hidden(FPSTR(T_SET_MQTT), FPSTR(T_DICT[lang][TD::D_MQTT]));
    interf->text(FPSTR(P_m_host), FPSTR(T_DICT[lang][TD::D_MQTT_Host]));
    interf->number(FPSTR(P_m_port), FPSTR(T_DICT[lang][TD::D_MQTT_Port]));
    interf->text(FPSTR(P_m_user), FPSTR(T_DICT[lang][TD::D_User]));
    interf->text(FPSTR(P_m_pass), FPSTR(T_DICT[lang][TD::D_Password]));
    interf->text(FPSTR(P_m_pref), FPSTR(T_DICT[lang][TD::D_MQTT_Topic]));
    interf->number(FPSTR(P_m_tupd), FPSTR(T_DICT[lang][TD::D_MQTT_Interval]));
    interf->button_submit(FPSTR(T_SET_MQTT), FPSTR(T_DICT[lang][TD::D_CONNECT]), FPSTR(P_GRAY));
    interf->json_section_end();

    interf->spacer();
    interf->button(FPSTR(T_SETTINGS), FPSTR(T_DICT[lang][TD::D_EXIT]));

    interf->json_section_end();

    interf->json_frame_flush();
}

/**
 *  BasicUI блок загрузки обновлений ПО
 */
void BasicUI::block_settings_update(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->json_section_hidden(FPSTR(T_DO_OTAUPD), FPSTR(T_DICT[lang][TD::D_Update]));
    interf->spacer(FPSTR(T_DICT[lang][TD::D_FWLOAD]));
    interf->file(FPSTR(T_DO_OTAUPD), FPSTR(T_DO_OTAUPD), FPSTR(T_DICT[lang][TD::D_UPLOAD]));
    interf->button(FPSTR(T_REBOOT), FPSTR(T_DICT[lang][TD::D_REBOOT]),!data?String(FPSTR(P_RED)):String(""));       // кнопка перехода в настройки времени
}

/**
 *  BasicUI блок настройки даты/времени
 */
void BasicUI::block_settings_time(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->json_frame_interface();

    // Headline
    interf->json_section_main(FPSTR(T_SET_TIME), FPSTR(T_DICT[lang][TD::D_DATETIME]));

    interf->comment(FPSTR(T_DICT[lang][TD::D_MSG_TZSet01]));     // комментарий-описание секции

    // сперва рисуем простое поле с текущим значением правил временной зоны из конфига
    interf->text(FPSTR(P_TZSET), FPSTR(T_DICT[lang][TD::D_MSG_TZONE]));

    // user-defined NTP server
    interf->text(FPSTR(P_userntp), FPSTR(T_DICT[lang][TD::D_NTP_Secondary]));
    // manual date and time setup
    interf->comment(FPSTR(T_DICT[lang][TD::D_MSG_DATETIME]));
    interf->text(FPSTR(P_DTIME), "", "", false);
    interf->hidden(FPSTR(P_DEVICEDATETIME),""); // скрытое поле для получения времени с устройства
    interf->button_submit(FPSTR(T_SET_TIME), FPSTR(T_DICT[lang][TD::D_SAVE]), FPSTR(P_GRAY));

    interf->spacer();

    // exit button
    interf->button(FPSTR(T_SETTINGS), FPSTR(T_DICT[lang][TD::D_EXIT]));

    // close and send frame
    interf->json_section_end();
    interf->json_frame_flush();

    // формируем и отправляем кадр с запросом подгрузки внешнего ресурса со списком правил временных зон
    // полученные данные заместят предыдущее поле выпадающим списком с данными о всех временных зонах
    interf->json_frame_custom(F("xload"));
    interf->json_section_content();
                    //id            val                         label   direct  skipl URL for external data
    interf->select(FPSTR(P_TZSET), embui.param(FPSTR(P_TZSET)), "",     false,  true, F("/js/tz.json"));
    interf->json_section_end();
    interf->json_frame_flush();

}

/**
 * Обработчик настроек WiFi в режиме клиента
 */
void BasicUI::set_settings_wifi(Interface *interf, JsonObject *data){
    if (!data) return;

    SETPARAM(FPSTR(P_hostname));        // сохраняем hostname в конфиг

    const char *ssid = (*data)[FPSTR(P_WCSSID)];    // переменные доступа в конфиге не храним
    const char *pwd = (*data)[FPSTR(P_WCPASS)];     // фреймворк хранит последнюю доступную точку самостоятельно

    WiFi.disconnect();
    if(ssid){
        embui.var(FPSTR(P_APonly),"0"); // сборосим режим принудительного AP, при попытке подключения к роутеру
        embui.save();
        embui.wifi_connect(ssid, pwd);
    } else {
        embui.wifi_connect();           // иницируем WiFi-подключение с новыми параметрами
        LOG(println, F("UI WiFi: No SSID defined!"));
    }

    section_settings_frame(interf, data);           // переходим в раздел "настройки"
}

/**
 * Обработчик настроек WiFi в режиме AP
 */
void BasicUI::set_settings_wifiAP(Interface *interf, JsonObject *data){
    if (!data) return;

    SETPARAM(FPSTR(P_hostname));    // эти переменные будут сохранены в конфиг-файл
    SETPARAM(FPSTR(P_APonly));
    SETPARAM(FPSTR(P_APpwd));

    embui.save();
    bool isAPmode = embui.param(FPSTR(P_APonly))=="1";
    if(isAPmode){
        embui.wifi_switchtoAP();
    } else {
        embui.wifi_connect(); 
    }
    section_settings_frame(interf, data);   // переходим в раздел "настройки"
}

/**
 * Обработчик настроек MQTT
 */
void BasicUI::set_settings_mqtt(Interface *interf, JsonObject *data){
    if (!data) return;
    // сохраняем настройки в конфиг
    SETPARAM(FPSTR(P_m_host));
    SETPARAM(FPSTR(P_m_port));
    SETPARAM(FPSTR(P_m_user));
    SETPARAM(FPSTR(P_m_pass));
    SETPARAM(FPSTR(P_m_pref));
    SETPARAM(FPSTR(P_m_tupd));
    //SETPARAM(FPSTR(P_m_tupd), some_mqtt_object.semqtt_int((*data)[FPSTR(P_m_tupd)]));

    embui.save();

    section_settings_frame(interf, data);
}

/**
 * Обработчик настроек даты/времени
 */
void BasicUI::set_settings_time(Interface *interf, JsonObject *data){
    if (!data) return;

    // Save and apply timezone rules
    String tzrule = (*data)[FPSTR(P_TZSET)];
    if (!tzrule.isEmpty()){
        SETPARAM(FPSTR(P_TZSET));
        embui.timeProcessor.tzsetup(tzrule.substring(4).c_str());   // cutoff '000_' prefix key
    }

    SETPARAM(FPSTR(P_userntp), embui.timeProcessor.setcustomntp((*data)[FPSTR(P_userntp)]));

    LOG(printf_P,PSTR("UI: devicedatetime=%s\n"),(*data)[FPSTR(P_DEVICEDATETIME)].as<String>().c_str());

    String datetime=(*data)[FPSTR(P_DTIME)];
    if (datetime.length())
        embui.timeProcessor.setTime(datetime);
    else if(!embui.sysData.wifi_sta) {
        datetime=(*data)[FPSTR(P_DEVICEDATETIME)].as<String>();
        if (datetime.length())
            embui.timeProcessor.setTime(datetime);
    }

    section_settings_frame(interf, data);
}

void BasicUI::set_language(Interface *interf, JsonObject *data){
        if (!data) return;

    //String _l= (*data)[FPSTR(P_LANGUAGE)];
    SETPARAM(FPSTR(P_LANGUAGE), lang = (*data)[FPSTR(P_LANGUAGE)].as<unsigned short>(); );
    section_settings_frame(interf, data);
}

void BasicUI::embuistatus(Interface *interf){
    if (!interf) return;
    interf->json_frame_value();
    interf->value(F("pTime"), embui.timeProcessor.getFormattedShortTime(), true);
    interf->value(F("pMem"), ESP.getFreeHeap(), true);
    interf->value(F("pUptime"), millis()/1000, true);
    interf->json_frame_flush();
}

void BasicUI::set_reboot(Interface *interf, JsonObject *data){
    if (!data) return;
    Task *t = new Task(TASK_SECOND*5, TASK_ONCE, nullptr, &ts, false, nullptr, [](){ LOG(println, F("Rebooting...")); delay(100); ESP.restart(); });
    t->enableDelayed();
    if(interf){
        interf->json_frame_interface();
        block_settings_update(interf, nullptr);
        interf->json_frame_flush();
    }
}

// stub function - переопределяется в пользовательском коде при необходимости добавить доп. пункты в меню настройки
void user_settings_frame(Interface *interf, JsonObject *data){};
