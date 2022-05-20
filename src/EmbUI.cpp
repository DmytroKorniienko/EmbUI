// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "EmbUI.h"
#include "ui.h"

// Update defs
#ifndef EMBUI_ESP_IMAGE_HEADER_MAGIC
 #define EMBUI_ESP_IMAGE_HEADER_MAGIC 0xE9
#endif

#ifndef EMBUI_GZ_HEADER
 #define EMBUI_GZ_HEADER 0x1F
#endif
uint8_t __attribute__((weak)) uploadProgress(size_t len, size_t total);
void mqtt_emptyFunction(const String &, const String &);

EmbUI *EmbUI::pInstance = nullptr;
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EMBUI)
EmbUI embui;
#endif

void section_main_frame(Interface *interf, JsonObject *data) {}
void pubCallback(Interface *interf){ LOG(println, F("weak pubCallback call"));}
String httpCallback(const String &param, const String &value, bool isSet) { return String(); }

//custom WS action handler, weak function
bool ws_action_handle(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    return false; // override this in user code if necessary
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
    if(ws_action_handle(server, client, type, arg, data, len)) return;

    if(type == WS_EVT_CONNECT){
        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        LOG(printf_P, PSTR("UI: ws[%s][%u] connect MEM: %u\n"), server->url(), client->id(), ESP.getFreeHeap());

        EmbUI::GetInstance()->sysData.isWSConnect = true; // на 5 секунд устанавливаем флаг
        Task *_t = new Task(TASK_SECOND * 5, TASK_ONCE, nullptr, &ts, false, nullptr, [](){TASK_RECYCLE; EmbUI::GetInstance()->sysData.isWSConnect = false;});
        _t->enableDelayed();

        Interface *interf = new Interface(EmbUI::GetInstance(), client);
        section_main_frame(interf, nullptr);
        Task *_t1 = new Task(TASK_SECOND, TASK_ONCE, nullptr, &ts, false, nullptr, [](){EmbUI::GetInstance()->send_pub(); TASK_RECYCLE;});
        _t1->enableDelayed();
        delete interf;

    } else
    if(type == WS_EVT_DISCONNECT){
        LOG(printf_P, PSTR("ws[%s][%u] disconnect\n"), server->url(), client->id());
    } else
    if(type == WS_EVT_ERROR){
        LOG(printf_P, PSTR("ws[%s][%u] error(%u): %s\n"), server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    } else
    if(type == WS_EVT_PONG){
        LOG(printf_P, PSTR("ws[%s][%u] pong[%u]: %s\n"), server->url(), client->id(), len, (len)?(char*)data:"");
    } else
    if(type == WS_EVT_DATA){
        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        bool pgkReady = false;
        if (info->len == len)
            pgkReady = true;
#ifdef EMBUI_USE_EXTERNAL_WS_BUFFER
        bool mPkg = false;
        static uint32_t lastLen = 0;
        if (info->len > EMBUI_USE_EXTERNAL_WS_BUFFER) {
            LOG(printf_P, PSTR("UI: ws package out of max package size %d byte \n"), EMBUI_USE_EXTERNAL_WS_BUFFER);
            return;
        }
        if (info->len != len){
            if (info->index == 0) {
                EmbUI::GetInstance()->clear_ext_ws_buff();
                EmbUI::GetInstance()->extWsBuf = new uint8_t[info->len];
                if (!EmbUI::GetInstance()->extWsBuf) {
                    LOG(printf_P, PSTR("UI: ws package out of mem size \n"));
                    return;
                }
                lastLen = 0;
            }
            if (lastLen != info->index || !EmbUI::GetInstance()->extWsBuf) {
                LOG(printf_P, PSTR("UI: ws package lost \n"));
                    EmbUI::GetInstance()->clear_ext_ws_buff();
                return;
            }
            memcpy(EmbUI::GetInstance()->extWsBuf+info->index, data, len);
            pgkReady = info->index + len == info->len && EmbUI::GetInstance()->extWsBuf ? true : false;
            lastLen += len;
            if (info->index + len == info->len) 
                mPkg = true;
        }
#endif
        if(info->final && pgkReady){
#ifdef EMBUI_USE_EXTERNAL_WS_BUFFER
            if (mPkg) {
                len = info->len;
                data = EmbUI::GetInstance()->extWsBuf;
            }
#endif
            if (!strncmp_P((const char *)data+1, PSTR("\"pkg\":\"post\""), 12)) {
                uint16_t objCnt = 2, tmpCnt=0; // минимально резервируем под 2 штуки
                for(uint16_t i=0; i<len; i++)
                    if(data[i]=='"')
                        tmpCnt++;
                objCnt += tmpCnt/4; // по кол-ву кавычек/4, т.к. "key":"value"
                LOG(printf_P, PSTR("UI: =POST= LEN: %u, obj: %u\n"), len, tmpCnt/4);
                DynamicJsonDocument *res = new DynamicJsonDocument(len + JSON_OBJECT_SIZE(objCnt)); // https://arduinojson.org/v6/assistant/
                if(!res) {
                    LOG(printf_P, PSTR("UI: res mem alloc error \n"));
#ifdef EMBUI_USE_EXTERNAL_WS_BUFFER
                    if (mPkg)
                        EmbUI::GetInstance()->clear_ext_ws_buff();
#endif
                    return;
                }
                    DeserializationError error = deserializeJson((*res), (const char*)data, len); // deserialize via copy to prevent dangling pointers in action()'s
#ifdef EMBUI_USE_EXTERNAL_WS_BUFFER
                if (mPkg) 
                    EmbUI::GetInstance()->clear_ext_ws_buff();          // очищаем буфер, т.к. уже не нужен
#endif
                if (error){
                    LOG(printf_P, PSTR("UI: Post deserialization err: %d\n"), error.code());
                    delete res;
                    return;                   
                }
                JsonObject data = (*res)[F("data")]; // ссылка на интересующую часть документа, документ обязан существовать до конца использования!
                EmbUI::GetInstance()->post(data);

                // Отправка эхо клиентам тут
                if (EmbUI::GetInstance()->ws.count()>1 && data){   // if there are multiple ws cliens connected, we must echo back data section, to reflect any changes
                    JsonObject &_d = data;
                    LOG(printf_P, PSTR("UI: =ECHO= MEM_1: %u\n"), _d.memoryUsage());
                    Interface *interf = new Interface(EmbUI::GetInstance(), &EmbUI::GetInstance()->ws, _d.memoryUsage() + 256);  // about 256 bytes requred for section structs
                    if(interf){
                        interf->json_frame_value();
                        for (JsonPair kv : _d) {
                            //interf->value(kv.key().c_str(),kv.value());
                            interf->value(String(kv.key().c_str()),kv.value().as<String>());
                        }
                        interf->json_frame_flush();
                        delete interf;
                    }
                } // else { LOG(println, F("NO DATA or less than 1 ws client")); }
                delete res;
            }
        }
    }
}

//custom HTTP API request parser, weak function
bool notfound_handle(AsyncWebServerRequest *request, const String& req)
{
    return false; // override this in user code if necessary
}

void notFound(AsyncWebServerRequest *request) {
    if(notfound_handle(request, request->url())) return;
    request->send(404, FPSTR(PGmimetxt), FPSTR(PG404));
}

void EmbUI::begin(){
    uint8_t retry_cnt = 3;

    // монтируем ФС только один раз при старте
#ifdef ESP32
    while(!LittleFS.begin(true)){
#else
    while(!LittleFS.begin()){
#endif
        LOG(println, F("UI: LittleFS initialization error, retrying..."));
        --retry_cnt;
        delay(100);
        if (!retry_cnt){
            LOG(println, F("FS dirty, I Give up!"));
            sysData.fsDirty = true;
        }
    }

    load();                 // try to load config from file
    cfgData.flags = param(FPSTR(P_cfgData)).toInt(); // restore bitmap data
    create_sysvars();       // create system variables (if missing)
    create_parameters();    // weak function, creates user-defined variables
    #ifdef EMBUI_USE_MQTT
    mqtt(param(FPSTR(P_m_pref)), param(FPSTR(P_m_host)), param(FPSTR(P_m_port)).toInt(), param(FPSTR(P_m_user)), param(FPSTR(P_m_pass)), mqtt_emptyFunction, false); // init mqtt
    #endif
    LOG(println, String(F("UI CONFIG: ")) + deb());
    #ifdef ESP8266
        e1 = WiFi.onStationModeGotIP(std::bind(&EmbUI::onSTAGotIP, this, std::placeholders::_1));
        e2 = WiFi.onStationModeDisconnected(std::bind(&EmbUI::onSTADisconnected, this, std::placeholders::_1));
        e3 = WiFi.onStationModeConnected(std::bind(&EmbUI::onSTAConnected, this, std::placeholders::_1));
        e4 = WiFi.onWiFiModeChange(std::bind(&EmbUI::onWiFiMode, this, std::placeholders::_1));
    #elif defined ESP32
        WiFi.onEvent(std::bind(&EmbUI::WiFiEvent, this, std::placeholders::_1, std::placeholders::_2));
        //WiFi.onEvent(std::bind(&TimeProcessor::WiFiEvent, &timeProcessor, std::placeholders::_1, std::placeholders::_2));
    #endif

    // восстанавливаем настройки времени
    timeProcessor.tzsetup(param(FPSTR(P_TZSET)).substring(4).c_str());
    timeProcessor.setcustomntp(param(FPSTR(P_userntp)).c_str());

    // запускаем WiFi
    wifi_init();
    
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

#ifdef EMBUI_USE_SSDP
    ssdp_begin(); LOG(println, F("Start SSDP"));
#endif

/*
#ifdef ESP32
  server.addHandler(new SPIFFSEditor(LittleFS, http_username,http_password));
#elif defined(ESP8266)
  //server.addHandler(new SPIFFSEditor(http_username,http_password, LittleFS));
  server.addHandler(new SPIFFSEditor(F("esp8266"),F("esp8266"), LittleFS));
#endif
*/

    server.on(PSTR("/version"), HTTP_ANY, [this](AsyncWebServerRequest *request) {
        String version;
        version.concat(F("EmbUI ver: "));
        version.concat(FPSTR(PGversion));
        version.concat('\n');
        version.concat(FPSTR(PGGITversion));
        request->send(200, FPSTR(PGmimetxt), version);
    });

    server.on(PSTR("/cmd"), HTTP_ANY, [this](AsyncWebServerRequest *request) {
        String result; 
        int params = request->params();
        for(int i=0;i<params;i++){
            AsyncWebParameter* p = request->getParam(i);
            if(p->isFile()){ //p->isPost() is also true
                //Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
            } else if(p->isPost()){
                //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
            } else {
                //Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                result = httpCallback(p->name(), p->value(), !p->value().isEmpty());
            }
        }
        request->send(200, FPSTR(PGmimetxt), result);
    });

    server.on(PSTR("/config"), HTTP_ANY, [this](AsyncWebServerRequest *request) {
        String config = deb();
        AsyncWebServerResponse *response = request->beginResponse(200, FPSTR(PGmimejson), config);

        response->addHeader(FPSTR(PGhdrcachec), FPSTR(PGnocache));
        request->send(response);
    });

/*
    // может пригодится позже, файлы отдаются как статика

    server.on(PSTR("/config.json"), HTTP_ANY, [this](AsyncWebServerRequest *request) {
        request->send(LittleFS, FPSTR(P_cfgfile), String(), true);
    });

    server.on(PSTR("/events_config.json"), HTTP_ANY, [this](AsyncWebServerRequest *request) {
        request->send(LittleFS, F("/events_config.json"), String(), true);
    });
*/
    // postponed reboot
    server.on(PSTR("/restart"), HTTP_ANY, [this](AsyncWebServerRequest *request) {
        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        Task *t = new Task(TASK_SECOND*5, TASK_ONCE, nullptr, &ts, false, nullptr, [](){ LOG(println, F("Rebooting...")); delay(100); ESP.restart(); });
        t->enableDelayed();
        request->redirect(F("/"));
    });

    server.on(PSTR("/heap"), HTTP_GET, [this](AsyncWebServerRequest *request){
        String out = String(F("Heap: "))+String(ESP.getFreeHeap());
#ifdef EMBUI_DEBUG
    #ifdef ESP8266
        out += String(F("\nFrac: ")) + String(ESP.getHeapFragmentation());
    #endif
        out += String(F("\nClient: ")) + String(ws.count());
#endif
        request->send(200, FPSTR(PGmimetxt), out);
    });

    // Simple Firmware Update Form
    server.on(PSTR("/update"), HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, FPSTR(PGmimehtml), F("<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' accept='.bin, .gz' name='update'><input type='submit' value='Update'></form>"));
    });

    server.on(PSTR("/update"), HTTP_POST, [this](AsyncWebServerRequest *request){
        if (Update.hasError()) {
            AsyncWebServerResponse *response = request->beginResponse(500, FPSTR(PGmimetxt), F("UPDATE FAILED"));
            response->addHeader(F("Connection"), F("close"));
            request->send(response);
            setPubInterval(EMBUI_PUB_PERIOD);
        } else {
            #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
                HeapSelectIram ephemeral;
            #endif
            Task *t = new Task(TASK_SECOND, TASK_ONCE, [](){ LOG(println, F("Rebooting...")); delay(100); ESP.restart(); }, &ts, false);
            t->enableDelayed();
            request->redirect(F("/"));
        }
    },[this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        setPubInterval(0); // отключить публикацию на время обновления
        if (!index) {
            #ifdef ESP8266
        	int type = (data[0] == EMBUI_ESP_IMAGE_HEADER_MAGIC || data[0] == EMBUI_GZ_HEADER)? U_FLASH : U_FS;
                Update.runAsync(true);
                // TODO: разобраться почему под littlefs образ генерится чуть больше чем размер доступной памяти по константам
                size_t size = (type == U_FLASH)? request->contentLength() : (uintptr_t)&_FS_end - (uintptr_t)&_FS_start;
            #endif
            #ifdef ESP32
                int type = (data[0] == EMBUI_ESP_IMAGE_HEADER_MAGIC)? U_FLASH : U_FS;
                size_t size = (type == U_FLASH)? request->contentLength() : UPDATE_SIZE_UNKNOWN;
            #endif
            LOG(printf_P, PSTR("Updating %s, file size:%u\n"), (type == U_FLASH)? "Firmware" : "Filesystem", request->contentLength());

            if (!Update.begin(size, type)) {
                Update.printError(Serial);
            }
        }
        if (!Update.hasError()) {
            if(Update.write(data, len) != len){
                Update.printError(Serial);
            }
        }
        if (final) {
            if(Update.end(true)){
                LOG(printf_P, PSTR("Update Success: %uB\n"), index+len);
            } else {
                Update.printError(Serial);
            }
            setPubInterval(EMBUI_PUB_PERIOD);
        }
        uploadProgress(index + len, request->contentLength());
    });

    //First request will return 0 results unless you start scan from somewhere else (loop/setup)
    //Do not request more often than 3-5 seconds
    server.on(PSTR("/scan"), HTTP_GET, [](AsyncWebServerRequest *request){
        String json = F("[");
        int n = WiFi.scanComplete();
        if(n == -2){
            WiFi.scanNetworks(true);
        } else if(n){
            for (int i = 0; i < n; ++i){
            if(i) json += F(",");
            json += F("{");
            json += String(F("\"rssi\":"))+String(WiFi.RSSI(i));
            json += String(F(",\"ssid\":\""))+WiFi.SSID(i)+F("\"");
            json += String(F(",\"bssid\":\""))+WiFi.BSSIDstr(i)+F("\"");
            json += String(F(",\"channel\":"))+String(WiFi.channel(i));
            json += String(F(",\"secure\":"))+String(WiFi.encryptionType(i));
#ifdef ESP8266
            json += String(F(",\"hidden\":"))+String(WiFi.isHidden(i)?FPSTR(P_true):FPSTR(P_false)); // что-то сломали и в esp32 больше не работает...
#endif
            json += F("}");
            }
            WiFi.scanDelete();
            if(WiFi.scanComplete() == -2){
            WiFi.scanNetworks(true);
            }
        }
        json += F("]");
        request->send(200, FPSTR(PGmimejson), json);
        json = String();
    });

    // server all files from LittleFS root
    server.serveStatic("/", LittleFS, "/")
        .setDefaultFile(PSTR("index.html"))
        .setCacheControl(PSTR("max-age=14400"));

    server.onNotFound(notFound);

    tHouseKeeper.set(TASK_SECOND, TASK_FOREVER, [this](){
#ifdef ESP8266
        MDNS.announce();
// #else
//         // https://github.com/espressif/arduino-esp32/issues/4406#issuecomment-829894157
        // struct ip4_addr addr;
        // addr.addr = 0;
        // mdns_query_a(param(FPSTR(P_hostname)).c_str(), 2000,  &addr);
#endif
        embui_uptime++;
        ws.cleanupClients(EMBUI_MAX_WS_CLIENTS);
        taskGC();
    } );
    ts.addTask(tHouseKeeper);
    tHouseKeeper.enableDelayed();

    server.begin();

    setPubInterval(EMBUI_PUB_PERIOD);

#ifdef EMBUI_USE_FTP
  //FTP Setup, ensure LittleFS is started before ftp;
  if (LittleFS.begin() && cfgData.isftp) {
    ftpSrv.begin(param(FPSTR(P_ftpuser)), param(FPSTR(P_ftppass))); //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
  }
#endif
}

/**
 * @brief - process posted data for the registered action
 * looks for registered action for the section name and calls the action with post data if found
 */
void EmbUI::post(JsonObject &data){
    section_handle_t *section = nullptr;

    for (JsonPair kv : data) {
        const char *kname = kv.key().c_str();
        for (int i = 0; !section && i < section_handle.size(); i++) {
            const char *sname = section_handle[i]->name.c_str();
            const char *mall = strchr(sname, '*');
            unsigned len = mall? mall - sname - 1 : strlen(kname);
            if (strncmp(sname, kname, len) == 0) {
                section = section_handle[i];
            }
        };
    }

    if (section) {
        LOG(printf_P, PSTR("\nUI: POST SECTION: %s\n\n"), section->name.c_str());
        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        Interface *interf = new Interface(this, &ws);
        section->callback(interf, &data);
        delete interf;
    }
}

void EmbUI::send_pub(){
    if (!ws.count()) return;
    #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
        HeapSelectIram ephemeral;
    #endif
    Interface *interf = new Interface(this, &ws, EMBUI_SMALL_JSON_SIZE);
    pubCallback(interf);
    delete interf;
}

void EmbUI::section_handle_remove(const String &name)
{
    for(int i=0; i<section_handle.size(); i++){
        if(section_handle.get(i)->name==name){
            delete section_handle.get(i);
            section_handle.remove(i);
            LOG(printf_P, PSTR("UI UNREGISTER: %s\n"), name.c_str());
            break;
        }
    }
}

void EmbUI::section_handle_add(const String &name, buttonCallback response)
{
    #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
        HeapSelectIram ephemeral;
    #endif
    section_handle_t *section = new section_handle_t;
    section->name = name;
    section->callback = response;
    section_handle.add(section);

    LOG(printf_P, PSTR("UI REGISTER: %s\n"), name.c_str());
}

/**
 * Возвращает указатель на строку со значением параметра из конфига
 * В случае отсутствующего параметра возвращает пустой указатель
 * (метод оставлен для совместимости)
 */
const char* EmbUI::param(const char* key)
{
    LOG(printf_P, PSTR("UI READ KEY: '%s'"), key);

    const char* value = cfg[key] | "";
    if (value){
        LOG(printf_P, PSTR(" value (%s)\n"), value);
    } else {
        LOG(println, F(" key is missing or not a *char\n"));
    }
    return value;
}

/**
 * обертка над param в виде String
 * В случае несуществующего ключа возвращает пустой String("")
 * TODO: эти методы толком не работают с объектами типа "не строка", нужна нормальная реализация с шаблонами и ДжейсонВариант
 */
String EmbUI::param(const String &key)
{
    LOG(printf_P, PSTR("UI READ KEY: '%s'"), key.c_str());
    String v;
    if (cfg[key].is<int>()){ v = cfg[key].as<int>(); }
    else if (cfg[key].is<float>()) { v = cfg[key].as<float>(); }
    else if (cfg[key].is<bool>()) { v = cfg[key].as<bool>(); }
    else { v = cfg[key] | ""; } // откат, все что не специальный тип, то пустая строка 

    LOG(printf_P, PSTR("string val (%s)\n"), v.c_str());
    return v;
}

String EmbUI::deb()
{
    String cfg_str;
    serializeJson(cfg, cfg_str);
    return cfg_str;
}

void EmbUI::led(uint8_t pin, bool invert){
    if (pin == 31) return;
    sysData.LED_PIN = pin;
    sysData.LED_INVERT = invert;
    pinMode(sysData.LED_PIN, OUTPUT);
}

void EmbUI::handle(){
#ifdef EMBUI_USE_MQTT
    mqtt_handle();
#endif
#ifdef EMBUI_USE_UDP
    udpLoop();
#endif
#ifdef ESP8266
    MDNS.update();
#endif
#ifdef EMBUI_USE_FTP
    if(EmbUI::GetInstance()->cfgData.isftp)
        ftpSrv.handleFTP();     //make sure in loop you call handleFTP()!!  
#endif
    //btn();
    //led_handle();
    ts.execute();           // run task scheduler
}

/**
 * метод для установки коллбеков на системные события, типа:
 * - WiFi подключиля/отключился
 * - получено время от NTP
 */
void EmbUI::set_callback(CallBack set, CallBack action, callback_function_t callback){

    switch (action){
        case CallBack::STAConnected :
            set ? _cb_STAConnected = std::move(callback) : _cb_STAConnected = nullptr;
            break;
        case CallBack::STADisconnected :
            set ? _cb_STADisconnected = std::move(callback) : _cb_STADisconnected = nullptr;
            break;
        case CallBack::STAGotIP :
            set ? _cb_STAGotIP = std::move(callback) : _cb_STAGotIP = nullptr;
            break;
        case CallBack::TimeSet :
            set ? timeProcessor.attach_callback(callback) : timeProcessor.dettach_callback();
            break;
        default:
            return;
    }
};

#ifdef EMBUI_USE_EXTERNAL_WS_BUFFER
void EmbUI::clear_ext_ws_buff(){
    if (extWsBuf) {
        delete[] extWsBuf;
        extWsBuf = nullptr;
    }
}
#endif

/**
 * call to create system-dependent variables,
 * both run-time and persistent
 */
void EmbUI::create_sysvars(){
    LOG(println, F("UI: Creating system vars"));
    var_create(FPSTR(P_cfgData), String("0"));        // bitmap data
    var_create(FPSTR(P_hostname), "");                // device hostname (autogenerated on first-run)
    var_create(FPSTR(P_WIFIMODE), String("0"));       // STA/AP/AP+STA, STA by default
    var_create(FPSTR(P_APpwd), F(TOSTRING(EMBUI_APPASSWORD)));  // пароль внутренней точки доступа
    // параметры подключения к MQTT
    var_create(FPSTR(P_m_host), "");                  // MQTT server hostname
    var_create(FPSTR(P_m_port), "");                  // MQTT port
    var_create(FPSTR(P_m_user), "");                  // MQTT login
    var_create(FPSTR(P_m_pass), "");                  // MQTT pass
    var_create(FPSTR(P_m_pref), mc);                  // MQTT topic == use ESP MAC address
    var_create(FPSTR(P_m_tupd), TOSTRING(EMBUI_MQTT_PUB_PERIOD));              // интервал отправки данных по MQTT в секундах
    // date/time related vars
    var_create(FPSTR(P_TZSET), "");                   // TimeZone/DST rule (empty value == GMT/no DST)
    var_create(FPSTR(P_userntp), "");                 // Backup NTP server
#ifdef EMBUI_USE_FTP
    var_create(FPSTR(P_ftpuser), FPSTR(P_FTP_USER_DEFAULT));                 // ftp user
    var_create(FPSTR(P_ftppass), FPSTR(P_FTP_PASS_DEFAULT));                 // ftp password
#endif
}

/**
 * @brief - set interval period for send_pub() task in seconds
 * 0 - will disable periodic task
 */
void EmbUI::setPubInterval(uint16_t _t){
    if(tValPublisher)
        tValPublisher->cancel(); // cancel & delete

    if (_t){
        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        tValPublisher = new Task(_t * TASK_SECOND, TASK_FOREVER, [this](){ if(ws.count()) send_pub(); }, &ts, true, nullptr, [this](){TASK_RECYCLE; tValPublisher=nullptr;});
    }
}

/**
 * @brief - restart config autosave timer
 * each call postpones cfg write to flash
 */
void EmbUI::autosave(bool force){
    if (force){
        tAutoSave.disable();
        save(nullptr, force);
    } else {
        tAutoSave.restartDelayed();
    }
};

const String EmbUI::getEmbUIver(){ return FPSTR(PGversion); }
const String EmbUI::getGITver(){ return FPSTR(PGGITversion); }