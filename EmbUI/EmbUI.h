// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#ifndef EmbUI_h
#define EmbUI_h

#include "globals.h"

#include <FS.h>

#ifdef ESP8266
 #include <ESPAsyncTCP.h>
 #include <LittleFS.h>
#endif

#ifdef ESP32
 #include <AsyncTCP.h>
 #include <LITTLEFS.h>
 #define FORMAT_LITTLEFS_IF_FAILED true
 #define LittleFS LITTLEFS
#endif

#include <ESPAsyncWebServer.h>
//#include <SPIFFSEditor.h>
#include <ArduinoJson.h>

#ifdef ESP8266
 #include <ESP8266mDNS.h>        // Include the mDNS library
 #ifdef USE_SSDP
  #include <ESP8266SSDP.h>
 #endif
#else
 #include <ESPmDNS.h>
 #include <Update.h>
 #ifdef USE_SSDP
  #include <ESP32SSDP.h>
 #endif
#endif

#include <Ticker.h>   // esp планировщик

#include <AsyncMqttClient.h>
#include "LList.h"

#include "timeProcessor.h"

#define AUTOSAVE_TIMEOUT    15      // configuration autosave timer, sec    (4 bit value)
#define UDP_PORT            4243    // UDP server port

#ifndef DELAY_AFTER_FS_WRITING
#define DELAY_AFTER_FS_WRITING       (50U)                        // 50мс, меньшие значения могут повлиять на стабильность
#endif

#ifndef __DISABLE_BUTTON0
#define __BUTTON 0 // Кнопка "FLASH" на NODE_MCU
#endif

#ifndef __IDPREFIX
#define __IDPREFIX F("EmbUI-")
#endif

#ifndef __CFGSIZE
#define __CFGSIZE (2048)
#endif


class Interface;

//-----------------------
#define TOGLE_STATE(val, curr) (val == FPSTR(P_true))? true : (val == FPSTR(P_false))? false : !curr;

#define SETPARAM(key, call...) if (data->containsKey(key)) { \
    embui.var(key, (*data)[key]); \
    call; \
}

#define CALL_SETTER(key, val, call) { \
    obj[key] = val; \
    call(nullptr, &obj); \
    obj.clear(); \
}

#define CALL_INTF(key, val, call) { \
    obj[key] = val; \
    Interface *interf = embui.ws.count()? new Interface(&embui, &embui.ws, 1000) : nullptr; \
    call(interf, &obj); \
    if (interf) { \
        interf->json_frame_value(); \
        interf->value(key, val, false); \
        interf->json_frame_flush(); \
        delete interf; \
    } \
}

#define CALL_INTF_OBJ(call) { \
    Interface *interf = embui.ws.count()? new Interface(&embui, &embui.ws, 1000) : nullptr; \
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

void __attribute__((weak)) section_main_frame(Interface *interf, JsonObject *data);
void __attribute__((weak)) pubCallback(Interface *interf);
String __attribute__((weak)) httpCallback(const String &param, const String &value, bool isset);
void __attribute__((weak)) uploadProgress(size_t len, size_t total);

//----------------------

#ifdef USE_SSDP
  #ifndef EXTERNAL_SSDP
    #define __SSDPNAME ("EmbUI (kDn)")
    #define __SSDPURLMODEL ("https://github.com/DmytroKorniienko/")
    #define __SSDPMODEL ("https://github.com/DmytroKorniienko/")
    #define __SSDPURLMANUF ("https://github.com/anton-zolotarev")
    #define __SSDPMANUF ("obliterator")
  #endif

  static const char PGnameModel[] PROGMEM = TOSTRING(__SSDPNAME);
  static const char PGurlModel[] PROGMEM = TOSTRING(__SSDPURLMODEL);
  static const char PGversion[] PROGMEM = TOSTRING(EMBUIVER);
  static const char PGurlManuf[] PROGMEM = TOSTRING(__SSDPURLMANUF);
  static const char PGnameManuf[] PROGMEM = TOSTRING(__SSDPMANUF);
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
    // оптимизация расхода памяти, все битовые флаги и другие потенциально "сжимаемые" переменные скидываем сюда
    #pragma pack(push,1)
    typedef union _BITFIELDS {
    struct {
        bool wifi_sta:1;    // флаг успешного подключения к внешней WiFi-AP, (TODO: переделать на события с коллбеками)
        bool mqtt_connected:1;
        bool mqtt_connect:1;
        bool mqtt_remotecontrol:1;

        bool mqtt_enable:1;
        bool isNeedSave:1;
        bool LED_INVERT:1;
        bool shouldReboot:1; // OTA update reboot flag
        uint8_t LED_PIN:5; // [0...30]
        uint8_t asave:4; // зачем так часто записывать конфиг? Ставлю раз в 15 секунд, вместо раза в секунду [0...15]
    };
    uint32_t flags; // набор битов для конфига
    _BITFIELDS() {
        wifi_sta = false;    // флаг успешного подключения к внешней WiFi-AP, (TODO: переделать на события с коллбеками)
        mqtt_connected = false;
        mqtt_connect = false;
        mqtt_remotecontrol = false;
        mqtt_enable = false;
        isNeedSave = false;
        LED_INVERT = false;
        shouldReboot = false; // OTA update reboot flag
        LED_PIN = 31; // [0...30]
        asave = AUTOSAVE_TIMEOUT; // зачем так часто записывать конфиг? Ставлю раз в 13 секунд, вместо раза в секунду [0...15]
    }
    } BITFIELDS;
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

  public:
    EmbUI() : cfg(__CFGSIZE), section_handle(), server(80), ws("/ws"){
      *mc='\0';
    }
    BITFIELDS sysData;
    AsyncWebServer server;
    AsyncWebSocket ws;
    mqttCallback onConnect;
    TimeProcessor timeProcessor;

    char mc[13]; // id из mac-адреса "ffffffffffff"

    void var(const String &key, const String &value, bool force = false);
    void var_create(const String &key, const String &value);
    void section_handle_add(const String &btn, buttonCallback response);
    const char* param(const char* key);
    String param(const String &key);
    bool isparamexists(const char* key){ return cfg.containsKey(key);}
    bool isparamexists(const String &key){ return cfg.containsKey(key);}
    void led(uint8_t pin, bool invert);
    String deb();
    void init();
    void begin();
    void handle();
    void autoSaveReset() {astimer = millis();} // передвинуть таймер
    void save(const char *_cfg = nullptr, bool force = false);
    void load(const char *_cfg = nullptr);
    void udp(const String &message);
    void udp();
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
    void mqtt_reconnect();
    void subscribe(const String &topic);
    void publish(const String &topic, const String &payload);
    void publish(const String &topic, const String &payload, bool retained);
    void remControl();
    void post(JsonObject data);
    void send_pub();
    String id(const String &tpoic);

    /**
     * Initialize WiFi using stored configuration
     */
    void wifi_init();

    /**
     * Подключение к WiFi AP в клиентском режиме
     */
    void wifi_connect(const char *ssid=nullptr, const char *pwd=nullptr);


    void set_callback(CallBack set, CallBack action, callback_function_t callback=nullptr);



  private:
    //void led_handle();        // пока убираю
    void led_on();
    void led_off();
    void led_inv();
    void autosave();
    void udpBegin();
    void udpLoop();
    void btn();
    void getAPmac();
    void pub_mqtt(const String &key, const String &value);
    void mqtt_handle();
    void subscribeAll(bool isOnlyGetSet=true);

    /**
      * устанавлием режим WiFi
      */
    void wifi_setmode(WiFiMode_t mode);

#ifdef ESP8266
    WiFiEventHandler e1, e2, e3;
    WiFiMode wifi_mode;           // используется в gpio led_handle (to be removed)
    void onSTAConnected(WiFiEventStationModeConnected ipInfo);
    void onSTAGotIP(WiFiEventStationModeGotIP ipInfo);
    void onSTADisconnected(WiFiEventStationModeDisconnected event_info);
#endif

#ifdef ESP32
    void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
#endif

    void connectToMqtt();
    void onMqttConnect();
    void setup_mDns();
    Ticker embuischedw;        // планировщик WiFi

    static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    static void onMqttSubscribe(uint16_t packetId, uint8_t qos);
    static void onMqttUnsubscribe(uint16_t packetId);
    static void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    static void onMqttPublish(uint16_t packetId);
    static void _onMqttConnect(bool sessionPresent);

    unsigned int localUdpPort = UDP_PORT;
    //char udpRemoteIP[16];
    String incomingPacket;
    String udpMessage; // буфер для сообщений Обмена по UDP
    unsigned long astimer;

    // callback pointers
    callback_function_t _cb_STAConnected = nullptr;
    callback_function_t _cb_STADisconnected = nullptr;
    callback_function_t _cb_STAGotIP = nullptr;

#ifdef USE_SSDP
    void ssdp_begin() {
          String hn = param(FPSTR(P_hostname));
          if (!hn.length())
              var(FPSTR(P_hostname), String(__IDPREFIX) + mc, true);

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
extern EmbUI embui;
#include "ui.h"
#endif