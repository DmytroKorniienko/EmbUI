// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#ifndef _EMBUI_EMBUI_H
#define _EMBUI_EMBUI_H

#include "globals.h"

#include <FS.h>

#ifdef ESP8266
 #include <ESPAsyncTCP.h>
 #include <LittleFS.h>
#else
 #include <AsyncTCP.h>
 #if ARDUINO <= 10805
  #include <LITTLEFS.h>
  #define LittleFS LITTLEFS
 #else
  #include <LittleFS.h>
 #endif
 #define U_FS   U_SPIFFS
#endif

#ifdef EMBUI_USE_FTP
#include <FTPServer.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#ifdef ESP8266
 #include <Updater.h>
 #include <ESP8266mDNS.h>        // Include the mDNS library
 #ifdef EMBUI_USE_SSDP
  #include <ESP8266SSDP.h>
 #endif
#endif

#ifdef ESP32
 #include <ESPmDNS.h>
 #include <Update.h>
 #ifdef EMBUI_USE_SSDP
  #include <ESP32SSDP.h>
 #endif
#endif

#include <AsyncMqttClient.h>
#include "LList.h"
#include "ts.h"
#include "timeProcessor.h"

#include "udpecho.h"

#ifndef EMBUI_PUB_PERIOD
#define EMBUI_PUB_PERIOD 10            // Values Publication period, s
#endif

#ifndef EMBUI_MQTT_PUB_PERIOD
#define EMBUI_MQTT_PUB_PERIOD     30
#endif

#define EMBUI_AUTOSAVE_TIMEOUT    2       // configuration autosave timer, sec    (4 bit value, multiplied by EMBUI_AUTOSAVE_MULTIPLIER)

#ifndef EMBUI_AUTOSAVE_MULTIPLIER
#define EMBUI_AUTOSAVE_MULTIPLIER       (10U)                           // множитель таймера автосохранения конфиг файла
#endif

//#define EMBUI_USE_SYS_BUTTON 0 // Кнопка "FLASH" на NODE_MCU

#ifndef EMBUI_IDPREFIX
#define EMBUI_IDPREFIX EmbUI-
#endif

#ifndef EMBUI_APPASSWORD
#define EMBUI_APPASSWORD
#endif

// size of a JsonDocument to hold EmbUI config 
#ifndef EMBUI_CFGSIZE
#define EMBUI_CFGSIZE (2048)
#endif

#ifndef EMBUI_MAX_WS_CLIENTS
#define EMBUI_MAX_WS_CLIENTS 4
#endif

#if defined(EMBUI_USE_EXTERNAL_WS_BUFFER) && EMBUI_USE_EXTERNAL_WS_BUFFER<2048
#undef EMBUI_USE_EXTERNAL_WS_BUFFER
#define EMBUI_USE_EXTERNAL_WS_BUFFER 2048
#endif

// TaskScheduler - Let the runner object be a global, single instance shared between object files.
extern Scheduler ts;

class Interface;

//-----------------------
#define TOGLE_STATE(val, curr) (val == "1")? true : (val == "0")? false : !curr;

#define SETPARAM(key, call...) if (data->containsKey(key)) { \
    EmbUI::GetInstance()->var(key, (*data)[key]); \
    call; \
}

#define CALL_SETTER(key, val, call) { \
    obj[key] = val; \
    call(nullptr, &obj); \
    obj.clear(); \
}


#if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP) //&& 1==0 // disabled for stability reason

#define CALL_INTF(key, val, call) { \
    { HeapSelectIram ephemeral; \
    obj[key] = val; \
    Interface *interf = EmbUI::GetInstance()->ws.count()? new Interface(EmbUI::GetInstance(), &EmbUI::GetInstance()->ws, EMBUI_SMALL_JSON_SIZE) : nullptr; \
    call(interf, &obj); \
    if (interf) { \
        interf->json_frame_value(); \
        interf->value(key, val, false); \
        interf->json_frame_flush(); \
        delete interf; \
    }} \
}

#define CALL_INTF_OBJ(call) { \
    { HeapSelectIram ephemeral; \
    Interface *interf = EmbUI::GetInstance()->ws.count()? new Interface(EmbUI::GetInstance(), &EmbUI::GetInstance()->ws, EMBUI_SMALL_JSON_SIZE*1.5) : nullptr; \
    call(interf, &obj); \
    if (interf) { \
        interf->json_frame_value(); \
        for (JsonPair kv : obj) { \
            interf->value(kv.key().c_str(), kv.value(), false); \
        } \
        interf->json_frame_flush(); \
        delete interf; \
    }} \
}

#define CALL_INTF_EMPTY(call) { \
    { HeapSelectIram ephemeral; \
    Interface *interf = EmbUI::GetInstance()->ws.count()? new Interface(EmbUI::GetInstance(), &EmbUI::GetInstance()->ws, EMBUI_SMALL_JSON_SIZE*1.5) : nullptr; \
    call(interf, nullptr); \
    if (interf) { \
        delete interf; \
    }} \
}

#else

#define CALL_INTF(key, val, call) { \
    obj[key] = val; \
    Interface *interf = EmbUI::GetInstance()->ws.count()? new Interface(EmbUI::GetInstance(), &EmbUI::GetInstance()->ws, EMBUI_SMALL_JSON_SIZE) : nullptr; \
    call(interf, &obj); \
    if (interf) { \
        interf->json_frame_value(); \
        interf->value(key, val, false); \
        interf->json_frame_flush(); \
        delete interf; \
    } \
}

#define CALL_INTF_OBJ(call) { \
    Interface *interf = EmbUI::GetInstance()->ws.count()? new Interface(EmbUI::GetInstance(), &EmbUI::GetInstance()->ws, EMBUI_SMALL_JSON_SIZE*1.5) : nullptr; \
    call(interf, &obj); \
    if (interf) { \
        interf->json_frame_value(); \
        for (JsonPair kv : obj) { \
            interf->value(kv.key().c_str(), kv.value(), false); \
        } \
        interf->json_frame_flush(); \
        delete interf; \
    } \
}

#define CALL_INTF_EMPTY(call) { \
    Interface *interf = EmbUI::GetInstance()->ws.count()? new Interface(EmbUI::GetInstance(), &EmbUI::GetInstance()->ws, EMBUI_SMALL_JSON_SIZE*1.5) : nullptr; \
    call(interf, nullptr); \
    if (interf) { \
        delete interf; \
    } \
}

#endif

void __attribute__((weak)) section_main_frame(Interface *interf, JsonObject *data);
void __attribute__((weak)) pubCallback(Interface *interf);
String __attribute__((weak)) httpCallback(const String &param, const String &value, bool isset);
void __attribute__((weak)) create_parameters();
bool __attribute__((weak)) notfound_handle(AsyncWebServerRequest *request, const String& req);
bool __attribute__((weak)) ws_action_handle(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

//----------------------

#ifdef EMBUI_USE_SSDP
    // #ifndef EMBUI_SSDP_NAME
    //     #define EMBUI_SSDP_NAME "EmbUI"
    // #endif
    
    #ifndef EMBUI_SSDP_MODEL
        #define EMBUI_SSDP_MODEL "EmbUI"
        static const char PGnameModel[] PROGMEM = EMBUI_SSDP_MODEL;
    #else
        static const char PGnameModel[] PROGMEM = TOSTRING(EMBUI_SSDP_MODEL);
    #endif

    #ifndef EMBUI_SSDP_URLMODEL
        #define EMBUI_SSDP_URLMODEL "https://github.com/DmytroKorniienko/EmbUI/"
        static const char PGurlModel[] PROGMEM = EMBUI_SSDP_URLMODEL;
    #else
        static const char PGurlModel[] PROGMEM = TOSTRING(EMBUI_SSDP_URLMODEL);
    #endif
    
    #ifndef EMBUI_SSDP_MANUF
        #define EMBUI_SSDP_MANUF "EmbUI Developers"
        static const char PGnameManuf[] PROGMEM = EMBUI_SSDP_MANUF;
    #else
        static const char PGnameManuf[] PROGMEM = TOSTRING(EMBUI_SSDP_MANUF);
    #endif
    #ifndef EMBUI_SSDP_URLMANUF
        #define EMBUI_SSDP_URLMANUF "https://github.com/DmytroKorniienko/EmbUI/"
        static const char PGurlManuf[] PROGMEM = EMBUI_SSDP_URLMANUF;
    #else
        static const char PGurlManuf[] PROGMEM = TOSTRING(EMBUI_SSDP_URLMANUF);
    #endif
#endif

// Callback enums
enum CallBack : uint8_t {
    detach = (0U),
    attach = (1U),
    STAConnected,
    STADisconnected,
    STAGotIP,
    TimeSet
};

class EmbUI
{
    static EmbUI *pInstance; // статический экземпляр класса, допускается лишь один экземпляр
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EMBUI)

#else
    EmbUI(EmbUI &other) = delete;
    void operator=(const EmbUI&) = delete;
#endif

    friend void mqtt_dummy_connect();
    // оптимизация расхода памяти, все битовые флаги и другие потенциально "сжимаемые" переменные скидываем сюда
    #pragma pack(push,4)
    typedef union _BITFIELDS {
    struct {
        bool wifi_sta:1;        // флаг успешного подключения к внешней WiFi-AP, (TODO: переделать на события с коллбеками)
        bool mqtt_connected:1;
        bool mqtt_connect:1;
        bool mqtt_remotecontrol:1;
        bool mqtt_enable:1;
        bool isWSConnect:1;     // было ли подключение WS в последние 5 секунд 
        bool LED_INVERT:1;
        bool fsDirty:1;         // флаг поврежденной FS (ошибка монтирования)
        bool isWiFiScanning:1;  // флаг процесса сканирования WiFi
        uint8_t LED_PIN:5;      // [0...30]
        uint8_t asave:4;        // 4 бита значения таймера автосохранения конфига (домножается на EMBUI_AUTOSAVE_MULTIPLIER)
    };
    uint32_t flags; // набор битов для конфига
    _BITFIELDS() {
        wifi_sta = false;    // флаг успешного подключения к внешней WiFi-AP, (TODO: переделать на события с коллбеками)
        mqtt_connected = false;
        mqtt_connect = false;
        mqtt_remotecontrol = false;
        mqtt_enable = false;
        LED_INVERT = false;
        isWSConnect = false;
        fsDirty = false;
        isWiFiScanning = false;
        LED_PIN = 31; // [0...30]
        asave = EMBUI_AUTOSAVE_TIMEOUT; // defaul timeout 2*10 sec
    }
    } BITFIELDS;
    #pragma pack(pop)

    #pragma pack(push,4)
    typedef union _EMBUICFG {
    struct {
        bool isftp:1;       // флаг использования FTP
    };
    uint32_t flags;         // набор битов для конфига (сохраняются в json)
    _EMBUICFG() {
        isftp = true;
    }
    } EMBUICFG;
    #pragma pack(pop)

    typedef void (*buttonCallback) (Interface *interf, JsonObject *data);
    typedef void (*mqttCallback) ();

    typedef struct section_handle_t{
      String name;
      buttonCallback callback;
    } section_handle_t;

    DynamicJsonDocument cfg;
    LList<section_handle_t*> section_handle;
    AsyncMqttClient mqttClient;

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EMBUI)
  public:
#else
  protected:
#endif
    EmbUI() : cfg(EMBUI_CFGSIZE), section_handle(), server(80), ws(F("/ws"))
#ifdef EMBUI_USE_FTP
    , ftpSrv(LittleFS)
#endif
    {
        memset(mc,0,sizeof(mc));

        tAutoSave.set(sysData.asave * EMBUI_AUTOSAVE_MULTIPLIER * TASK_SECOND, TASK_ONCE, [this](){LOG(println, F("UI: AutoSave")); save();} );    // config autosave timer
        ts.addTask(tAutoSave);
        EmbUI::pInstance = this;
    }

  public:

    ~EmbUI(){
        ts.deleteTask(tAutoSave);
        ts.deleteTask(*tValPublisher);
        ts.deleteTask(tHouseKeeper);
        EmbUI::pInstance = nullptr;
    }

    static EmbUI *GetInstance() {
        if(pInstance==nullptr){
            #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
                HeapSelectIram ephemeral;
            #endif
            pInstance = new EmbUI();
        }
        return pInstance;
    }

    static const String getEmbUIver();
    static const String getGITver();

    BITFIELDS sysData;
    EMBUICFG cfgData;
    AsyncWebServer server;
    AsyncWebSocket ws;
    mqttCallback onConnect;
    TimeProcessor timeProcessor;
#ifdef EMBUI_USE_FTP
    FTPServer ftpSrv;
#endif
#ifdef EMBUI_USE_EXTERNAL_WS_BUFFER    
    uint8_t *extWsBuf = nullptr;
    void clear_ext_ws_buff();
#endif

    char mc[13]; // id (hex) из mac-адреса "ffffffffffff"

    void section_handle_add(const String &btn, buttonCallback response);
    void section_handle_remove(const String &name);
    const char* param(const char* key);
    String param(const String &key);
    bool isparamexists(const char* key){ return cfg.containsKey(key);}
    bool isparamexists(const String &key){ return cfg.containsKey(key);}
    void led(uint8_t pin, bool invert);
    String deb();
    void init();
    void begin();
    void handle();
    void save(const char *_cfg = nullptr, bool force = false);
    void load(const char *cfgfile = nullptr);   // if null, than default cfg file is used
    //  * tries to load json file from FS and deserialize it into provided DynamicJsonDocument, returns false on error
    bool loadjson(const char *filepath, DynamicJsonDocument &obj);
    void udp(const String &message);
    void udp();

#ifdef EMBUI_USE_MQTT
    // MQTT
    bool isMQTTconected() { return sysData.mqtt_connected; }
    void pub_mqtt(const String &key, const String &value);
    void mqtt_handle();
    void subscribeAll(bool isOnlyGetSet=true);
    void mqtt_reconnect();
    void mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), bool remotecontrol);
    void mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload));
    void mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload));
    void mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), bool remotecontrol);
    void mqtt(const String &host, int port, const String &user, const String &pass, bool remotecontrol);
    void mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, bool remotecontrol);
    void mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) (), bool remotecontrol);
    void mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) ());
    void mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) ());
    void mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) (), bool remotecontrol);
    void mqtt(void (*mqttFunction) (const String &topic, const String &payload), bool remotecontrol);
    void mqtt(void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) (), bool remotecontrol);
    void mqttReconnect();
    void subscribe(const String &topic);
    void publish(const String &topic, const String &payload);
    void publish(const String &topic, const String &payload, bool retained);
    void publishto(const String &topic, const String &payload, bool retained);
    void remControl();

    String id(const String &tpoic);
#endif

    void send_pub();

    /**
     * @brief - process posted data for the registered action
     * if post came from the WebUI echoes received data back to the WebUI,
     * if post came from some other place - sends data to the WebUI
     * looks for registered action for the section name and calls the action with post data if found
     */
    void post(JsonObject &data);

    /**
     * Initialize WiFi using stored configuration
     */
    void wifi_init();

    /**
     * Подключение к WiFi AP в клиентском режиме
     */
    void wifi_connect(const String &ssid="", const String &pwd="");

    /**
     * метод для установки коллбеков на системные события, типа:
     * - WiFi подключиля/отключился
     * - получено время от NTP
     */
    void set_callback(CallBack set, CallBack action, callback_function_t callback=nullptr);

    /**
     * @brief - set interval period for send_pub() task in seconds
     * 0 - will disable periodic task
     */
    void setPubInterval(uint16_t _t);


    /**
     * @brief - set variable's value in the system config object
     * @param key - variable's key
     * @param value - value to set
     * @param force - register new key in config if it does not exist
     * Note: by default if key has not been registerred on init it won't be created
     * beware of dangling pointers here passing non-static char*, use JsonVariant or String instead 
     */
    template <typename T> void var(const String &key, const T& value, bool force = false){
            if (!force && !cfg.containsKey(key)) {
            LOG(printf_P, PSTR("UI ERR: KEY (%s) is NOT initialized!\n"), key.c_str());
            return;
        }

        String _v=value;
        if ((cfg.capacity() - cfg.memoryUsage()) < _v.length()+16){
            // cfg is out of mem, try to compact it
            //size_t mem = cfg.memoryUsage();
            cfg.garbageCollect();
            //LOG(printf_P, PSTR("UI: cfg garbage cleaned: %u, free %u\n"), mem - cfg.memoryUsage(), cfg.capacity() - cfg.memoryUsage());
            LOG(printf_P, PSTR("UI: cfg garbage cleanup: %u free out of %u\n"), cfg.capacity() - cfg.memoryUsage(), cfg.capacity());
        }

        if (cfg[key].set(value)){
            LOG(printf_P, PSTR("UI cfg WRITE key:'%s' val:'%s...', cfg mem free: %d\n"), key.c_str(), _v.substring(0, 15).c_str(), cfg.capacity() - cfg.memoryUsage());
            autosave();
            return;
        }

        LOG(printf_P, PSTR("UI ERR: KEY (%s), cfg out of mem!\n"), key.c_str());
    }

    /**
     * @brief - create varialbe template
     * it accepts types suitable to be added to the ArduinoJson cfg document used as a dictionary
     */
    template <typename T> void var_create(const String &key, const T& value){
        if(cfg[key].isNull()){
            cfg[key].set(value);
            LOG(printf_P, PSTR("UI CREATE key: (%s) value: (%s) RAM: %d\n"), key.c_str(), String(value).substring(0, 15).c_str(), ESP.getFreeHeap());
            autosave();
        }
    };

    /**
     * @brief - initialize/restart config autosave timer
     * each call postpones cfg write to flash
     */
    void autosave(bool force = false);

    /**
     * Recycler for dynamic tasks
     */
    void taskRecycle(Task *t) {GC_taskRecycle(t);}

    // WiFi-related
    /**
      * устанавлием режим WiFi в AP
      */
    void wifi_switchtoAP() {
        LOG(println, F("UI WiFi: Force AP mode"));
        WiFi.enableAP(true);    // включаю AP
        WiFi.enableSTA(false);  // отключаю STA
    }

    unsigned long getUptime() {return embui_uptime;}

  private:
    /**
     * call to create system-dependent variables,
     * both run-time and persistent
     */ 
    void create_sysvars();
    void led_on();
    void led_off();
    void led_inv();

#ifdef EMBUI_USE_UDP
    AsyncUDP Udp;
    bool udpApply = false;
    unsigned int localUdpPort = EMBUI_UDP_PORT;
    //char udpRemoteIP[16];
    String incomingPacket;
    String udpMessage; // буфер для сообщений Обмена по UDP
    void udpBegin();
    void udpLoop();
#endif
#ifdef EMBUI_USE_SYS_BUTTON
    void btn();
#endif
    void getAPmac();

    // Scheduler tasks
    Task *embuischedw = nullptr;       // WiFi reconnection helper
    Task *tValPublisher = nullptr;     // Status data publisher
    Task tHouseKeeper;     // Maintenance task, runs every second
    Task tAutoSave;          // config autosave timer

    unsigned long embui_uptime = 0; // uptime in seconds

    // WiFi-related
    /**
      * устанавлием режим WiFi
      */
    void wifi_setmode(WiFiMode_t mode);

#ifdef ESP8266
    WiFiEventHandler e1, e2, e3, e4;
    WiFiMode wifi_mode;           // используется в gpio led_handle (to be removed)
    void onSTAConnected(WiFiEventStationModeConnected ipInfo);
    void onSTAGotIP(WiFiEventStationModeGotIP ipInfo);
    void onSTADisconnected(WiFiEventStationModeDisconnected event_info);
    void onWiFiMode(WiFiEventModeChange event_info);
#endif

#ifdef ESP32
    typedef void (*WIFISCANCB)(int); // wifi scan completed callback
public:
    //WIFISCANCB getWiFiScanCB() {return pf_wifiscan;}
    void setWiFiScanCB(WIFISCANCB _f) {pf_wifiscan = _f;}
private:
    void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
    WIFISCANCB pf_wifiscan;
#endif

#ifdef EMBUI_USE_MQTT
    // MQTT Private Methods and vars
    String m_pref; // к сожалению они нужны, т.к. в клиент передаются указатели на уже имеющийся объект, значит на конфиг ссылку отдавать нельзя!!!
    String m_host;
    String m_port;
    String m_user;
    String m_pass;
    String m_will;
    void connectToMqtt();
    void onMqttConnect();
    static void _onMqttConnect(bool sessionPresent);
    static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    static void onMqttSubscribe(uint16_t packetId, uint8_t qos);
    static void onMqttUnsubscribe(uint16_t packetId);
    static void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    static void onMqttPublish(uint16_t packetId);
#endif

    unsigned long astimer;

    // callback pointers
    callback_function_t _cb_STAConnected = nullptr;
    callback_function_t _cb_STADisconnected = nullptr;
    callback_function_t _cb_STAGotIP = nullptr;

    void setup_mDns();

    // Dyn tasks garbage collector
    void taskGC() {GC_taskGC();}

#ifdef EMBUI_USE_SSDP
    void ssdp_begin() {
          String hn = param(FPSTR(P_hostname));
          if (!hn.length())
              var(FPSTR(P_hostname), String(F(TOSTRING(EMBUI_IDPREFIX))) + mc, true);

          uint32_t chipId;
          #ifdef ESP32
              chipId = ESP.getEfuseMac();
          #else
              chipId = ESP.getChipId();    
          #endif  
          SSDP.setDeviceType(F("upnp:rootdevice"));
          SSDP.setSchemaURL(F("description.xml"));
          SSDP.setHTTPPort(80);
          SSDP.setName(param(FPSTR(P_hostname)));
          SSDP.setSerialNumber(String(chipId));
          SSDP.setURL(F("/"));
          SSDP.setModelName(FPSTR(PGnameModel));
          SSDP.setModelNumber(FPSTR(PGversion));
          SSDP.setModelURL(String(F("http://"))+(WiFi.status() != WL_CONNECTED ? WiFi.softAPIP().toString() : WiFi.localIP().toString()));
          SSDP.setManufacturer(FPSTR(PGnameManuf));
          SSDP.setManufacturerURL(FPSTR(PGurlManuf));
          SSDP.begin();

          (&server)->on(PSTR("/description.xml"), HTTP_GET, [&](AsyncWebServerRequest *request){
            request->send(200, FPSTR(PGmimexml), getSSDPSchema());
          });
    }
    
    String getSSDPSchema() {
        uint32_t chipId;
        #ifdef ESP32
            chipId = ESP.getEfuseMac();
        #else
            chipId = ESP.getChipId();    
        #endif  
      String s = "";
        s +=F("<?xml version=\"1.0\"?>\n");
        s +=F("<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n");
        s +=F("\t<specVersion>\n");
        s +=F("\t\t<major>1</major>\n");
        s +=F("\t\t<minor>0</minor>\n");
        s +=F("\t</specVersion>\n");
        s +=F("<URLBase>");
        s +=String(F("http://"))+(WiFi.status() != WL_CONNECTED ? WiFi.softAPIP().toString() : WiFi.localIP().toString());
        s +=F("</URLBase>");
        s +=F("<device>\n");
        s +=F("\t<deviceType>upnp:rootdevice</deviceType>\n");
        s +=F("\t<friendlyName>");
        s += param(F("hostname"));
        s +=F("</friendlyName>\r\n");
        s +=F("\t<presentationURL>index.html</presentationURL>\r\n");
        s +=F("\t<serialNumber>");
        s += String(chipId);
        s +=F("</serialNumber>\r\n");
        s +=F("\t<modelName>");
        s += FPSTR(PGnameModel);
        s +=F("</modelName>\r\n");
        s +=F("\t<modelNumber>");
        s += FPSTR(PGversion);
        s +=F("</modelNumber>\r\n");
        s +=F("\t<modelURL>");
        s += FPSTR(PGurlModel);
        s +=F("</modelURL>\r\n");
        s +=F("\t<manufacturer>");
        s += FPSTR(PGnameManuf);
        s +=F("</manufacturer>\r\n");
        s +=F("\t<manufacturerURL>");
        s += FPSTR(PGurlManuf);
        s +=F("</manufacturerURL>\r\n");
        //s +=F("\t<UDN>0543bd4e-53c2-4f33-8a25-1f75583a19a2");
        s +=F("\t<UDN>0543bd4e-53c2-4f33-8a25-1f7558");
        char cn[7];
        sprintf_P(cn, PSTR("%02x%02x%02x"), ((chipId >> 16) & 0xff), ((chipId >>  8) & 0xff), chipId & 0xff);
        s += cn;
        s +=F("</UDN>\r\n");
        s +=F("\t</device>\n");
        s +=F("</root>\r\n\r\n");
      return s;
    }
#endif
};

// Глобальный объект фреймворка
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EMBUI)
extern EmbUI embui;
#endif

#include "ui.h"
#endif