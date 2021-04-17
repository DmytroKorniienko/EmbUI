// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "EmbUI.h"
#include "ui.h"

#ifdef EMBUI_DEBUG
 #include "MemoryInfo.h"
#endif


// Update defs
#ifndef ESP_IMAGE_HEADER_MAGIC
 #define ESP_IMAGE_HEADER_MAGIC 0xE9
#endif

#ifndef GZ_HEADER
 #define GZ_HEADER 0x1F
#endif

void mqtt_emptyFunction(const String &, const String &);

EmbUI embui;

void section_main_frame(Interface *interf, JsonObject *data) {}
void pubCallback(Interface *interf){}
String httpCallback(const String &param, const String &value, bool isSet) { return String(); }

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
    if(type == WS_EVT_CONNECT){
        LOG(printf_P, PSTR("UI: ws[%s][%u] connect MEM: %u\n"), server->url(), client->id(), ESP.getFreeHeap());

        Interface *interf = new Interface(&embui, client);
        section_main_frame(interf, nullptr);
        embui.send_pub();
        delete interf;

    } else
    if(type == WS_EVT_DISCONNECT){
        LOG(printf_P, PSTR("ws[%s][%u] disconnect\n"), server->url(), client->id());
    } else
    if(type == WS_EVT_ERROR){
        LOG(printf_P, PSTR("ws[%s][%u] error(%u): %s\n"), server->url(), client->id(), *((uint16_t*)arg), (char*)data);
        httpCallback(F("sys_WS_EVT_ERROR"), "", false); // сообщим об ошибке сокета
    } else
    if(type == WS_EVT_PONG){
        LOG(printf_P, PSTR("ws[%s][%u] pong[%u]: %s\n"), server->url(), client->id(), len, (len)?(char*)data:"");
    } else
    if(type == WS_EVT_DATA){
        AwsFrameInfo *info = (AwsFrameInfo*)arg;
        if(info->final && info->index == 0 && info->len == len){
            LOG(printf_P, PSTR("UI: =POST= LEN: %u\n"), len);

            DynamicJsonDocument *doc = new DynamicJsonDocument(2*len + 32);
            DeserializationError error = deserializeJson((*doc), (const char*)data, info->len); // deserialize via copy to prevent dangling pointers in action()'s
            if (error){
                LOG(printf_P, PSTR("UI: Post deserialization err: %d\n"), error.code());
                return;
            }

            if ((*doc)[FPSTR(P_pkg)] && (*doc)[FPSTR(P_pkg)] == F("post")) {
                doc->shrinkToFit();      // this doc should not grow anyway
                //LOG(printf_P, PSTR("UI: =POST= MEM_1: %u\n"), doc->memoryUsage());

                if (embui.ws.count()>1 && (*doc)[F("data")]){   // if there are multiple ws cliens connected, we must echo back data section, to reflect any changes
                    DynamicJsonDocument *echo = new DynamicJsonDocument(len+32);
                    DeserializationError error = deserializeJson((*echo), data, info->len); // deserialize via zero-copy, it will be released once data copied to ws buffer
                    if (error){
                        LOG(printf_P, PSTR("UI: Echo deserialization err: %d\n"), error.code());
                    } else {
                        JsonObject _d = (*echo)[F("data")];
                        LOG(printf_P, PSTR("UI: =ECHO= MEM_1: %u\n"), _d.memoryUsage());
                        Interface *interf = new Interface(&embui, &embui.ws, len+128);      // about 128 bytes requred for section structs
                        interf->json_frame_value();
                        interf->value(_d);
                        interf->json_frame_flush();
                        delete interf;
                        delete echo;
                    }
                } // else { LOG(println, F("NO DATA or less than 1 ws client")); }

                // откладываем обработку и освобождаем буфера ws
                new Task(100, TASK_ONCE, nullptr, &ts, true, nullptr, [doc](){
                    JsonObject data = (*doc)[F("data")];
                    embui.post(data);
                    delete doc;
                    TASK_RECYCLE;
                });
            }
        }
    }
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, FPSTR(PGmimetxt), FPSTR(PG404));
}

void EmbUI::begin(){

    uint8_t retry_cnt = 3;

    // монтируем ФС только один раз при старте
    while(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        LOG(println, F("UI: LittleFS initialization error, retrying..."));
        --retry_cnt;
        delay(100);
        if (!retry_cnt){
            LOG(println, F("FS dirty, I Give up!"));
            fsDirty = true;
        }
    }

    load();                 // try to load config from file
    create_sysvars();       // create system variables (if missing)
    create_parameters();    // weak function, creates user-defined variables
    mqtt(param(FPSTR(P_m_pref)), param(FPSTR(P_m_host)), param(FPSTR(P_m_port)).toInt(), param(FPSTR(P_m_user)), param(FPSTR(P_m_pass)), mqtt_emptyFunction, false); // init mqtt

    LOG(println, String(F("UI CONFIG: ")) + embui.deb());
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

#ifdef USE_SSDP
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
        request->send(200, FPSTR(PGmimetxt), F("EmbUI ver: " TOSTRING(EMBUIVER)));
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
        new Task(TASK_SECOND, TASK_ONCE, nullptr, &ts, true, nullptr, [](){ LOG(println, F("Rebooting...")); delay(100); ESP.restart(); });
        request->redirect(F("/"));
    });

    server.on(PSTR("/heap"), HTTP_GET, [this](AsyncWebServerRequest *request){
        String out = String(F("Heap: "))+String(ESP.getFreeHeap());
#ifdef EMBUI_DEBUG
    #ifdef ESP8266
        out += String(F("\nFrac: ")) + String(getFragmentation());
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
        } else {
            Task *t = new Task(TASK_SECOND, TASK_ONCE, [](){ LOG(println, F("Rebooting...")); delay(100); ESP.restart(); }, &ts, false);
            t->enableDelayed();
            request->redirect(F("/"));
        }
    },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        if (!index) {
            #ifdef ESP8266
        	int type = (data[0] == ESP_IMAGE_HEADER_MAGIC || data[0] == GZ_HEADER)? U_FLASH : U_FS;
                Update.runAsync(true);
                // TODO: разобраться почему под littlefs образ генерится чуть больше чем размер доступной памяти по константам
                size_t size = (type == U_FLASH)? request->contentLength() : (uintptr_t)&_FS_end - (uintptr_t)&_FS_start;
            #endif
            #ifdef ESP32
                int type = (data[0] == ESP_IMAGE_HEADER_MAGIC)? U_FLASH : U_FS;
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

    server.begin();

    setPubInterval(PUB_PERIOD);

    tHouseKeeper.set(TASK_SECOND, TASK_FOREVER, [this](){
            ws.cleanupClients(MAX_WS_CLIENTS);
            #ifdef ESP8266
                MDNS.update();
            #endif
            taskGC();
        } );
    ts.addTask(tHouseKeeper);
    tHouseKeeper.enableDelayed();
}

/**
 * @brief - process posted data for the registered action
 * looks for registered action for the section name and calls the action with post data if found
 */
void EmbUI::post(JsonObject data){
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
        Interface *interf = new Interface(this, &ws);
        section->callback(interf, &data);
        delete interf;
    }
}

void EmbUI::send_pub(){
    if (!ws.count()) return;
    Interface *interf = new Interface(this, &ws, 512);
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
    mqtt_handle();
    udpLoop();
    ts.execute();           // run task scheduler
    //btn();
    //led_handle();
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

/*
 * OTA update progress
 */
uint8_t uploadProgress(size_t len, size_t total){
    static int prev = 0;
    float part = total / 25.0;  // logger chunks
    int curr = len / part;
    uint8_t progress = 100*len/total;
    if (curr != prev) {
        prev = curr;
        LOG(printf_P, PSTR("%u%%.."), progress );
    }
    return progress;
}

/**
 * call to create system-dependent variables,
 * both run-time and persistent
 */
void EmbUI::create_sysvars(){
    LOG(println, F("UI: Creating system vars"));
    var_create(FPSTR(P_hostname), "");                // device hostname (autogenerated on first-run)
    var_create(FPSTR(P_APonly),  FPSTR(P_false));     // режим AP-only (только точка доступа), не трогать
    var_create(FPSTR(P_APpwd), "");                   // пароль внутренней точки доступа
    // параметры подключения к MQTT
    var_create(FPSTR(P_m_host), "");                   // MQTT server hostname
    var_create(FPSTR(P_m_port), "");                   // MQTT port
    var_create(FPSTR(P_m_user), "");                   // MQTT login
    var_create(FPSTR(P_m_pass), "");                   // MQTT pass
    var_create(FPSTR(P_m_pref), embui.mc);             // MQTT topic == use ESP MAC address
    var_create(FPSTR(P_m_tupd), TOSTRING(MQTT_PUB_PERIOD));              // интервал отправки данных по MQTT в секундах
    // date/time related vars
    var_create(FPSTR(P_TZSET), "");                   // TimeZone/DST rule (empty value == GMT/no DST)
    var_create(FPSTR(P_userntp), "");                 // Backup NTP server
}

/**
 * @brief - set interval period for send_pub() task in seconds
 * 0 - will disable periodic task
 */
void EmbUI::setPubInterval(uint16_t _t){
    if(tValPublisher)
        tValPublisher->cancel(); // cancel & delete

    if (_t){
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

void EmbUI::taskRecycle(Task *t){
    if (!taskTrash)
        taskTrash = new std::vector<Task*>(8);

    taskTrash->emplace_back(t);
}

// Dyn tasks garbage collector
void EmbUI::taskGC(){
    if (!taskTrash || taskTrash->empty())
        return;

    //size_t heapbefore = ESP.getFreeHeap();
    for(auto& _t : *taskTrash) { delete _t; }

    delete taskTrash;
    taskTrash = nullptr;
    //LOG(printf_P, PSTR("UI: task garbage collect: released %d bytes\n"), ESP.getFreeHeap() - heapbefore);
}
