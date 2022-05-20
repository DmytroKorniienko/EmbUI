#include "basicui.h"
//#include "ts.h"

    bool BasicUI::isBackOn = true;    // is returning to main settings? default=true
#ifndef ESP8266     
    Task *BasicUI::_WIFIScan = nullptr;
#endif
/**
 * Define configuration variables and controls handlers
 * 
 * Variables has literal names and are kept within json-configuration file on flash
 * Control handlers are bound by literal name with a particular method. This method is invoked
 * by manipulating controls
 * 
 */
void BasicUI::add_sections(bool skipBack){ // is returning to main settings skipped?
    LOG(println, F("UI: Creating webui vars"));

    isBackOn = !skipBack;

    /**
     * обработчики действий
     */ 
    // вывод BasicUI секций
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_SETTINGS), section_settings_frame);    // generate "settings" UI section
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_SH_NETW), block_settings_netw);        // generate "network settings" UI section
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_SH_TIME), block_settings_time);         // generate "time settings" UI section
    //EmbUI::GetInstance()->section_handle_add(FPSTR(T_SH_OTHER), show_settings_other);

    // обработка базовых настроек
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_SET_WIFI), set_settings_wifi);         // обработка настроек WiFi
#ifdef EMBUI_USE_MQTT
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_SET_MQTT), set_settings_mqtt);         // обработка настроек MQTT
#endif
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_SET_SCAN), set_scan_wifi);             // обработка сканирования WiFi
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_SET_TIME), set_settings_time);         // установки даты/времени
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_REBOOT), set_reboot);                  // перезагрузка

#ifdef EMBUI_USE_FTP
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_SET_FTP), set_ftp);                    // обработка настроек FTP
    EmbUI::GetInstance()->section_handle_add(FPSTR(T_CHK_FTP), set_chk_ftp);                // обработка переключателя FTP
#endif
    //EmbUI::GetInstance()->section_handle_add(FPSTR(T_004B), set_settings_other);
}

/**
 * This code adds "Settings" section to the MENU
 * it is up to you to properly open/close Interface menu json_section
 */
void BasicUI::opt_setup(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->option(FPSTR(T_SETTINGS), FPSTR(TD_SETTINGS));     // пункт меню "настройки"
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

    interf->json_section_main(FPSTR(T_SETTINGS), FPSTR(TD_SETTINGS));

    interf->button(FPSTR(T_SH_NETW), FPSTR(TD_WIFI_MQTT));  // кнопка перехода в настройки сети
    interf->button(FPSTR(T_SH_TIME), FPSTR(TD_TIME));       // кнопка перехода в настройки времени

    // call for user_defined function that may add more elements to the "settings page"
    user_settings_frame(interf, data);

    interf->spacer();
    block_settings_update(interf, data);                                     // добавляем блок интерфейса "обновления ПО"

    interf->json_section_end();
    interf->json_frame_flush();
}

// Блок настроек WiFi
void BasicUI::block_only_wifi(Interface *interf, JsonObject *data) {
    interf->spacer(FPSTR(TD_WIFIAPOPTS));
    interf->select(String(FPSTR(P_WIFIMODE)), EmbUI::GetInstance()->param(FPSTR(P_WIFIMODE)), String(FPSTR(TD_WIFIMODE)));
        interf->option("0", String(FPSTR(TD_WIFI_STA)));
        interf->option("1", String(FPSTR(TD_WIFI_AP)));
        interf->option("2", String(FPSTR(TD_WIFI_APSTA)));
    interf->json_section_end();

    interf->comment(FPSTR(TD_MSG_WIFIMODE));

    interf->text(FPSTR(P_hostname), FPSTR(TD_HOSTNAME));
    interf->password(FPSTR(P_APpwd),  FPSTR(TD_MSG_APPROTECT));

    interf->spacer(FPSTR(TD_WIFICLIENTOPTS));
    interf->json_section_line(FPSTR(T_LOAD_WIFI));
        interf->select_edit(FPSTR(P_WCSSID), String(WiFi.SSID()), String(FPSTR(TD_WIFISSID)));
        interf->json_section_end();
        interf->button(FPSTR(T_SET_SCAN), FPSTR(TD_SCAN), FPSTR(P_GREEN), 22);
    interf->json_section_end();
    interf->password(FPSTR(P_WCPASS), String(""), FPSTR(TD_PASSWORD));
    interf->button_submit(FPSTR(T_SET_WIFI), FPSTR(TD_CONNECT), FPSTR(P_GRAY));
}

/**
 *  BasicUI блок интерфейса настроек WiFi/MQTT
 */
void BasicUI::block_settings_netw(Interface *interf, JsonObject *data){
    if (!interf) return;
    
    interf->json_frame_interface();

    // Headline
    interf->json_section_main(FPSTR(T_OPT_NETW), FPSTR(TD_WIFI_MQTT));

    // форма настроек Wi-Fi Client
    interf->json_section_hidden(FPSTR(T_SET_WIFI), FPSTR(TD_WIFICLIENT));
        block_only_wifi(interf, data);
    interf->json_section_end();

#ifdef EMBUI_USE_MQTT
    // форма настроек MQTT
    interf->json_section_hidden(FPSTR(T_SET_MQTT), FPSTR(TD_MQTT));
    interf->text(FPSTR(P_m_host), FPSTR(TD_MQTT_HOST));
    interf->number(FPSTR(P_m_port), FPSTR(TD_MQTT_PORT));
    interf->text(FPSTR(P_m_user), FPSTR(TD_USER));
    interf->password(FPSTR(P_m_pass), FPSTR(TD_PASSWORD));
    interf->text(FPSTR(P_m_pref), FPSTR(TD_MQTT_PREFIX));
    interf->number(FPSTR(P_m_tupd), FPSTR(TD_MQTT_INTERVAL));
    interf->button_submit(FPSTR(T_SET_MQTT), FPSTR(TD_CONNECT), FPSTR(P_GRAY));
    interf->json_section_end();
#endif

#ifdef EMBUI_USE_FTP
    // форма настроек FTP
    interf->json_section_hidden("H", FPSTR(TD_FTP));
        interf->json_section_begin("C", "");
            interf->checkbox(FPSTR(T_CHK_FTP), String(EmbUI::GetInstance()->cfgData.isftp), FPSTR(TD_FTP), true);
        interf->json_section_end();
        interf->json_section_begin(FPSTR(T_SET_FTP), "");
            interf->text(FPSTR(P_ftpuser), FPSTR(TD_USER));
            interf->password(FPSTR(P_ftppass), FPSTR(TD_PASSWORD));
            interf->button_submit(FPSTR(T_SET_FTP), FPSTR(TD_SAVE), FPSTR(P_GRAY));
        interf->json_section_end();
    interf->json_section_end();
#endif

    interf->spacer();
    interf->button(FPSTR(T_SETTINGS), FPSTR(TD_EXIT));

    interf->json_section_end();

    interf->json_frame_flush();

    if(!EmbUI::GetInstance()->sysData.isWiFiScanning){ // автосканирование при входе в настройки
        EmbUI::GetInstance()->sysData.isWiFiScanning = true;
        set_scan_wifi(interf, data);
    }
}

/**
 *  BasicUI блок загрузки обновлений ПО
 */
void BasicUI::block_settings_update(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->json_section_hidden(FPSTR(T_DO_OTAUPD), FPSTR(TD_UPDATE));
    interf->spacer(FPSTR(TD_FWLOAD));
    interf->file(FPSTR(T_DO_OTAUPD), FPSTR(T_DO_OTAUPD), FPSTR(TD_UPLOAD));
    interf->button_confirm(FPSTR(T_REBOOT), FPSTR(TD_REBOOT), FPSTR(TD_MSG_CONF), !data?String(FPSTR(P_RED)):String("")); // кнопка перезагрузки
}

/**
 *  BasicUI блок настройки даты/времени
 */
void BasicUI::block_settings_time(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->json_frame_interface();

    // Headline
    interf->json_section_main(FPSTR(T_SET_TIME), FPSTR(TD_DATETIME));

    interf->comment(FPSTR(TD_MSG_TZSET01));     // комментарий-описание секции

    // сперва рисуем простое поле с текущим значением правил временной зоны из конфига
    interf->text(FPSTR(P_TZSET), FPSTR(TD_MSG_TZONE));

    // user-defined NTP server
    interf->text(FPSTR(P_userntp), FPSTR(TD_NTP_SECONDARY));
    // manual date and time setup
    interf->comment(FPSTR(TD_MSG_DATETIME));
    interf->datetime(FPSTR(P_DTIME), String(""), true);
    interf->hidden(FPSTR(P_DEVICEDATETIME),""); // скрытое поле для получения времени с устройства
    interf->button_submit(FPSTR(T_SET_TIME), FPSTR(TD_SAVE), FPSTR(P_GRAY));

    interf->spacer();

    // exit button
    interf->button(FPSTR(T_SETTINGS), FPSTR(TD_EXIT));

    // close and send frame
    interf->json_section_end();
    interf->json_frame_flush();

    // формируем и отправляем кадр с запросом подгрузки внешнего ресурса со списком правил временных зон
    // полученные данные заместят предыдущее поле выпадающим списком с данными о всех временных зонах
    interf->json_frame_custom(FPSTR(T_XLOAD));
    interf->json_section_content();
                    //id            val                         label   direct  skipl URL for external data
    interf->select(FPSTR(P_TZSET), EmbUI::GetInstance()->param(FPSTR(P_TZSET)), "",     false,  true, F("/js/tz.json"));
    interf->json_section_end();
    interf->json_frame_flush();
}

/**
 * Обработчик настроек WiFi
 */
void BasicUI::set_settings_wifi(Interface *interf, JsonObject *data){
    if (!data || EmbUI::GetInstance()->sysData.isWSConnect) return;

    SETPARAM(FPSTR(P_hostname));        // сохраняем hostname в конфиг
    SETPARAM(FPSTR(P_APpwd));
    SETPARAM(FPSTR(P_WIFIMODE));
    EmbUI::GetInstance()->save();

    uint8_t wifiMode = EmbUI::GetInstance()->param(FPSTR(P_WIFIMODE)).toInt();

    String ssid = (*data)[FPSTR(P_WCSSID)];    // переменные доступа в конфиге не храним
    String pwd = (*data)[FPSTR(P_WCPASS)];     // фреймворк хранит последнюю доступную точку самостоятельно

    if(wifiMode==1){
        EmbUI::GetInstance()->wifi_switchtoAP();
    } else {
        if(wifiMode==0)
            WiFi.mode(WIFI_STA);
        else
            WiFi.mode(WIFI_AP_STA);

        LOG(printf_P, PSTR("UI WiFi: Connecting to %s\n"), ssid.c_str());
        if((!ssid.isEmpty() && !pwd.isEmpty()) || (!ssid.isEmpty() && WiFi.SSID()!=ssid))
            EmbUI::GetInstance()->wifi_connect(ssid, pwd);
        else
            EmbUI::GetInstance()->wifi_connect(); 
    }

    if(isBackOn) section_settings_frame(interf, data);            // переходим в раздел "настройки"
}

#ifdef EMBUI_USE_MQTT
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
    //SETPARAM(FPSTR(P_m_tupd), some_mqtt_object.semqtt_int((*data)[FPSTR(P_m_tupd)));

    EmbUI::GetInstance()->save();

    if(isBackOn) section_settings_frame(interf, data); 
}
#endif

/**
 * Обработчик сканирования WiFi
 */
void BasicUI::set_scan_wifi(Interface *interf, JsonObject *data){
    if (!interf) return;

    if (WiFi.scanComplete() == -2) {
        LOG(printf_P, PSTR("UI WiFi: WiFi scan starting\n"));
        interf->json_frame_custom(FPSTR(T_XLOAD));
        interf->json_section_content();
        interf->constant(FPSTR(T_SET_SCAN), FPSTR(TD_SCAN), true, FPSTR(P_GREEN), 22);
        interf->json_section_end();
        interf->json_frame_flush();

        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        Task *t = new Task(300, TASK_ONCE, nullptr, &ts, false, nullptr, [](){
            EmbUI::GetInstance()->sysData.isWiFiScanning = true;
            #ifdef ESP8266
            WiFi.scanNetworksAsync(scan_complete);     // Сканируем с коллбеком, по завершению скана запустится scan_complete()
            #endif
            #ifdef ESP32
            EmbUI::GetInstance()->setWiFiScanCB(&scan_complete);
            WiFi.scanNetworks(true);         // У ESP нет метода с коллбеком, поэтому просто сканируем
            #endif
            TASK_RECYCLE;
        });
        t->enableDelayed();
    }
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
        EmbUI::GetInstance()->timeProcessor.tzsetup(tzrule.substring(4).c_str());   // cutoff '000_' prefix key
    }

    SETPARAM(FPSTR(P_userntp), EmbUI::GetInstance()->timeProcessor.setcustomntp((*data)[FPSTR(P_userntp)]));

    LOG(printf_P,PSTR("UI: devicedatetime=%s\n"),(*data)[FPSTR(P_DEVICEDATETIME)].as<String>().c_str());

    String datetime=(*data)[FPSTR(P_DTIME)];
    if (datetime.length())
        EmbUI::GetInstance()->timeProcessor.setTime(datetime);
    else if(!EmbUI::GetInstance()->sysData.wifi_sta) {
        datetime=(*data)[FPSTR(P_DEVICEDATETIME)].as<String>();
        if (datetime.length())
            EmbUI::GetInstance()->timeProcessor.setTime(datetime);
    }

    if(isBackOn) section_settings_frame(interf, data); 
}

void BasicUI::embuistatus(Interface *interf){
    if (!interf) return;
    interf->json_frame_value();
    interf->value(F("pTime"), EmbUI::GetInstance()->timeProcessor.getFormattedShortTime(), true);

#if !defined(ESP32) || !defined(BOARD_HAS_PSRAM)
    #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
        uint32_t iram;
        uint32_t dram;
        {
            HeapSelectIram ephemeral;
            iram = ESP.getFreeHeap();
        }
        {
            HeapSelectDram ephemeral;
            dram = ESP.getFreeHeap();
        }
        interf->value(F("pMem"), String(dram)+" / "+String(iram), true);
    #else
        interf->value(F("pMem"), String(ESP.getFreeHeap()), true);
    #endif
#else
    if(psramFound()){
        interf->value(F("pMem"), String(ESP.getFreeHeap())+F(" / ")+String(ESP.getFreePsram()), true);
        //LOG(printf_P, PSTR("Free PSRAM: %d\n"), ESP.getFreePsram());
    } else {
        interf->value(F("pMem"), String(ESP.getFreeHeap()), true);
    }
#endif

    //interf->value(F("pUptime"), String(EmbUI::GetInstance()->getUptime()), true);
    char fuptime[16];
    uint32_t tm = EmbUI::GetInstance()->getUptime();
    sprintf_P(fuptime, PSTR("%u.%02u:%02u:%02u"),tm/86400,(tm/3600)%24,(tm/60)%60,tm%60);
    interf->value(F("pUptime"), String(fuptime), true);
    interf->json_frame_flush();
}

#ifdef EMBUI_USE_FTP
void BasicUI::set_ftp(Interface *interf, JsonObject *data){
    if (!data) return;

    String user = (*data)[FPSTR(P_ftpuser)];
    String pass = (*data)[FPSTR(P_ftppass)];

    SETPARAM(FPSTR(P_ftpuser));
    SETPARAM(FPSTR(P_ftppass));

    BasicUI::set_chk_ftp(interf, data);

    //EmbUI::GetInstance()->ftpSrv.stop();
    //EmbUI::GetInstance()->ftpSrv.begin(user, pass);
    //EmbUI::GetInstance()->save();
    if(isBackOn) section_settings_frame(interf, data); 
}

void BasicUI::set_chk_ftp(Interface *interf, JsonObject *data){
    if (!data) return;

    if(data->containsKey(FPSTR(T_CHK_FTP))){
        EmbUI::GetInstance()->cfgData.isftp = (*data)[FPSTR(T_CHK_FTP)]=="1";
    }

    EmbUI::GetInstance()->var(FPSTR(P_cfgData), String(EmbUI::GetInstance()->cfgData.flags));

    if(EmbUI::GetInstance()->cfgData.isftp){
        EmbUI::GetInstance()->ftpSrv.stop();
        EmbUI::GetInstance()->ftpSrv.begin(EmbUI::GetInstance()->param(FPSTR(P_ftpuser)), EmbUI::GetInstance()->param(FPSTR(P_ftppass)));
    } else
        EmbUI::GetInstance()->ftpSrv.stop();
    //EmbUI::GetInstance()->save();
}
#endif

void BasicUI::set_reboot(Interface *interf, JsonObject *data){
    if (!data) return;
    bool isReboot = !EmbUI::GetInstance()->sysData.isWSConnect;
    if(isReboot){
        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        Task *t = new Task(TASK_SECOND*5, TASK_ONCE, nullptr, &ts, false, nullptr, [](){ LOG(println, F("Rebooting...")); delay(100); ESP.restart(); });
        t->enableDelayed();
    }
    if(interf){
        interf->json_frame_interface();
        block_settings_update(interf, isReboot?nullptr:data);
        interf->json_frame_flush();
    }
}

// stub function - переопределяется в пользовательском коде при необходимости добавить доп. пункты в меню настройки
void user_settings_frame(Interface *interf, JsonObject *data){};

// после завершения сканирования обновляем список WiFi
void BasicUI::scan_complete(int n){
    #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
        HeapSelectIram ephemeral;
    #endif
    Interface *interf = EmbUI::GetInstance()->ws.count()? new Interface(EmbUI::GetInstance(), &EmbUI::GetInstance()->ws) : nullptr;
    LOG(printf_P, PSTR("UI WiFi: Scan complete %d networks found\n"), n);
    if(interf){
        interf->json_frame_interface();
        interf->json_section_line(FPSTR(T_LOAD_WIFI));
        String ssid = WiFi.SSID();
        interf->select_edit(FPSTR(P_WCSSID), ssid, String(FPSTR(TD_WIFISSID)));
        for (int i = 0; i < WiFi.scanComplete(); i++) {
            interf->option(WiFi.SSID(i), WiFi.SSID(i));
            LOG(printf_P, PSTR("UI WiFi: WiFi Net %s\n"), WiFi.SSID(i).c_str());
        }
        if(ssid.isEmpty())
            interf->option("", ""); // at the end of list
        interf->json_section_end();
        interf->button(FPSTR(T_SET_SCAN), FPSTR(TD_SCAN), FPSTR(P_GREEN), 22);
        interf->json_section_end();
        interf->json_frame_flush();

        delete interf;
    }
    Task *_t = new Task(
        TASK_SECOND,
        TASK_ONCE, [](){
            if (WiFi.scanComplete() >= 0) {
                EmbUI::GetInstance()->sysData.isWiFiScanning = false;
                WiFi.scanDelete();
                LOG(printf_P, PSTR("UI WiFi: Scan List deleted\n"));
            }
        },
        &ts, false);
    _t->enableDelayed();
}

void BasicUI::show_progress(Interface *interf, JsonObject *data){
    if (!interf) return;

    interf->json_frame_interface();
    interf->json_section_begin(FPSTR(T_DO_OTAUPD));
    interf->constant("U1", String(FPSTR(TD_UPDATE)));
    interf->constant("U2", (*data)[FPSTR(T_UPROGRESS)].as<String>() + String("%"), true);
    interf->json_section_end();
    interf->json_frame_flush();
}

/*
 * OTA update progress
 */
uint8_t uploadProgress(size_t len, size_t total){
    DynamicJsonDocument doc(256);
    JsonObject obj = doc.to<JsonObject>();
    static int prev = 0;
    float part = total / 25.0;  // logger chunks
    int curr = len / part;
    uint8_t progress = 100*len/total;
    if (curr != prev) {
        prev = curr;
        LOG(printf_P, PSTR("%u%%.."), progress );
        obj[FPSTR(T_UPROGRESS)] = String(progress);
        CALL_INTF_OBJ(BasicUI::show_progress);
    }
    return progress;
}