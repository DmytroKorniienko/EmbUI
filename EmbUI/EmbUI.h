// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#ifndef EmbUI_h
#define EmbUI_h

#include "Arduino.h"

#ifdef ESP8266
#include <ESPAsyncTCP.h>
#include <FS.h>
#include "LittleFS.h"
#else
#include <AsyncTCP.h>
#include "LittleFS.h"
#endif

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include "ArduinoJson.h"

#ifdef ESP8266
#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
#include <ESP8266mDNS.h>        // Include the mDNS library
#ifdef USE_SSDP
#include <ESP8266SSDP.h>
#endif
#else
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Update.h>
#ifdef USE_SSDP
#include <ESP32SSDP.h>
#endif
#endif

#include <Ticker.h>   // esp планировщик

#include <AsyncMqttClient.h>
#include "LList.h"

// STRING Macro
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

class Interface;

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

//-----------------------
#define TOGLE_STATE(val, curr) (val == F("true"))? true : (val == F("false"))? false : !curr;

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

#if defined(EMBUI_DEBUG) && DEBUG_TELNET_OUTPUT
	//#define LOG                   telnet
	#define LOG(func, ...) telnet.func(__VA_ARGS__)
#elif defined(EMBUI_DEBUG)
	//#define LOG                   Serial
	#define LOG(func, ...) Serial.func(__VA_ARGS__)
#else
	#define LOG(func, ...) ;
#endif

void __attribute__((weak)) section_main_frame(Interface *interf, JsonObject *data);
void __attribute__((weak)) pubCallback(Interface *interf);
void __attribute__((weak)) httpCallback(const String &param, const String &value);
void __attribute__((weak)) uploadProgress(size_t len, size_t total);

//----------------------

static const char PGmimetxt[] PROGMEM  = "text/plain";
static const char PGmimecss[] PROGMEM  = "text/css";
static const char PGmimexml[] PROGMEM  = "text/css";
static const char PGmimehtml[] PROGMEM = "text/html; charset=utf-8";
static const char PGmimejson[] PROGMEM = "application/json";
static const char PGhdrcontentenc[] PROGMEM = "Content-Encoding";
static const char PGhdrcachec[] PROGMEM = "Cache-Control";
static const char PGgzip[] PROGMEM = "gzip";
static const char PGnocache[] PROGMEM = "no-cache, no-store, must-revalidate";    // 10 days cache

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

class EmbUI
{
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
//      *ip='\0';
      *mc='\0';
//      *mac='\0';
      *m_pref='\0';
      *m_host='\0';
      *m_user='\0';
      *m_pass='\0';
      *m_host='\0';
      *udpRemoteIP='\0';
      *incomingPacket='\0';
      *udpMessage='\0';
    }

    AsyncWebServer server;
    AsyncWebSocket ws;

    void var(const String &key, const String &value, bool force = false);
    void var_create(const String &key, const String &value);
    void section_handle_add(const String &btn, buttonCallback response);
    const char* param(const char* key);
    String param(const String &key);
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

    mqttCallback onConnect;

    void mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) (), bool remotecontrol);
    void mqtt(const String &pref, const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) ());
    void mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) ());
    void mqtt(const String &host, int port, const String &user, const String &pass, void (*mqttFunction) (const String &topic, const String &payload), void (*mqttConnect) (), bool remotecontrol);
    void mqtt_reconnect();

    void subscribe(const String &topic);
    void publish(const String &topic, const String &payload);
    void publish(const String &topic, const String &payload, bool retained);

    void remControl();

    /**
     * Подключение к WiFi AP в клиентском режиме
     */
    void wifi_connect(const char *ssid=nullptr, const char *pwd=nullptr);
    bool wifi_sta = false;    // флаг успешного подключения к внешней WiFi-AP, (TODO: переделать на события с коллбеками)

    void post(JsonObject data);
    void send_pub();

    char mc[13]; // id из mac-адреса "ffffffffffff"

    String id(const String &tpoic);

    static char m_pref[16]; // префикс MQTT
    char m_host[256]; // MQTT
    int m_port = 0;
    char m_user[64];
    char m_pass[64];

  private:
    void led_handle();
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
    bool mqtt_enable = false;

    void subscribeAll();

    char udpRemoteIP[16];
    unsigned int localUdpPort = 4243;
    char incomingPacket[64];

    unsigned int asave = 30000; // зачем так часто записывать конфиг? Ставлю раз в 30 секунд, вместо раза в секунду
    bool isNeedSave = false;
    unsigned long astimer;

    // WiFi related
    WiFiEventHandler e1, e2, e3;
    WiFiMode wifi_mode;   // используется в gpio led_handle (to be removed)
    void onSTAConnected(WiFiEventStationModeConnected ipInfo);
    void onSTAGotIP(WiFiEventStationModeGotIP ipInfo);
    void onSTADisconnected(WiFiEventStationModeDisconnected event_info);
    void setup_mDns();
#ifdef USE_SSDP
    void ssdp_begin() {
          String hn = param(F("hostname"));
          if (!hn.length())
              var(F("hostname"), String(__IDPREFIX) + mc, true);

          uint32_t chipId;
          #ifdef ESP32
              chipId = ESP.getEfuseMac();
          #else
              chipId = ESP.getChipId();    
          #endif  
          SSDP.setDeviceType(F("upnp:rootdevice"));
          SSDP.setSchemaURL(F("description.xml"));
          SSDP.setHTTPPort(80);
          SSDP.setName(param(F("hostname")));
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
    /**
      * устанавлием режим WiFi
      */
    void wifi_setmode(WiFiMode mode);

    int LED_PIN = -1;
    bool LED_INVERT = false;
    char udpMessage[65]; // буфер для сообщений Обмена по UDP

    void connectToMqtt();
    void onMqttConnect();
    static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    static void onMqttSubscribe(uint16_t packetId, uint8_t qos);
    static void onMqttUnsubscribe(uint16_t packetId);
    static void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    static void onMqttPublish(uint16_t packetId);
    static void _onMqttConnect(bool sessionPresent);

    static bool mqtt_connected;
    static bool mqtt_connect;
    static bool mqtt_remotecontrol;

    Ticker embuischedw;        // планировщик WiFi
};

// Глобальный объект фреймворка
extern EmbUI embui;
#include "ui.h"
#endif