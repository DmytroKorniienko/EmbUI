// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "EmbUI.h"
#include "wi-fi.h"

#ifdef ESP8266
#include "user_interface.h"
void EmbUI::onSTAConnected(WiFiEventStationModeConnected ipInfo)
{
    LOG(printf_P, PSTR("UI WiFi: connected to %s\r\n"), ipInfo.ssid.c_str());
    if(_cb_STAConnected)
        _cb_STAConnected();        // execule callback
}

void EmbUI::onSTAGotIP(WiFiEventStationModeGotIP ipInfo)
{
    sysData.wifi_sta = true;
    embuischedw.detach();
    LOG(printf_P, PSTR("WiFi: Got IP: %s\r\n"), ipInfo.ip.toString().c_str());
    wifi_setmode(WIFI_STA);            // Shutdown internal Access Point
    setup_mDns();
    timeProcessor.onSTAGotIP(ipInfo);
    if(_cb_STAGotIP)
        _cb_STAGotIP();        // execule callback
}

void EmbUI::onSTADisconnected(WiFiEventStationModeDisconnected event_info)
{
    LOG(printf_P, PSTR("UI WiFi: Disconnected from SSID: %s, reason: %d\n"), event_info.ssid.c_str(), event_info.reason);
    sysData.wifi_sta = false;       // to be removed and replaced with API-method

    if (embuischedw.active()){
        LOG(println, F("UI WiFi: embuischedw running, bailing out..."));
        return;
    }

    /*
      esp8266 сильно тормозит в комбинированном режиме AP-STA при постоянных попытках реконнекта, WEB-интерфейс становится
      неотзывчивым, сложно изменять настройки.
      В качестве решения переключаем контроллер в режим AP-only после WIFI_CONNECT_TIMEOUT таймаута на попытку переподключения.
      Далее делаем периодические попытки переподключений каждые WIFI_RECONNECT_TIMER секунд
    */
    embuischedw.once_scheduled(WIFI_CONNECT_TIMEOUT, [this](){
        LOG(println, F("UI WiFi: switching to internal AP"));
        wifi_setmode(WIFI_AP);
        embuischedw.once_scheduled(WIFI_RECONNECT_TIMER, [this](){ embuischedw.detach(); wifi_setmode(WIFI_AP_STA); WiFi.begin();} );
    } );

    timeProcessor.onSTADisconnected(event_info);
    if(_cb_STADisconnected)
        _cb_STADisconnected();        // execute callback
}

#endif  //ESP8266

#ifdef ESP32
// need to test it under ESP32 (might not need any scheduler to handle both Client and AP at the same time)
void EmbUI::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)   // , WiFiEventInfo_t info
{
    switch (event){
    case SYSTEM_EVENT_AP_START:
        LOG(println, F("UI WiFi: Access-point started"));
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        LOG(println, F("UI WiFi: Station connected"));
        if(_cb_STAConnected)
            _cb_STAConnected();        // execule callback
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        WiFi.mode(WIFI_STA);            // Shutdown internal Access Point
        LOG(printf_P, PSTR("UI WiFi: Connected to '%s', IP: "), WiFi.SSID().c_str());  // IPAddress(info.got_ip.ip_info.ip.addr)
        LOG(println, WiFi.localIP());

        if(WiFi.getMode() != WIFI_MODE_STA){    // Switch to STA only mode once IP obtained
            WiFi.mode(WIFI_MODE_STA);         
            LOG(println, F("UI WiFi: switch to STA mode"));
        }
        if(_cb_STAGotIP)
            _cb_STAGotIP();        // execule callback
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        LOG(printf_P, PSTR("UI WiFi: Disconnected, reason: %d\n"), info.disconnected.reason);
        // https://github.com/espressif/arduino-esp32/blob/master/tools/sdk/include/esp32/esp_wifi_types.h
        if(WiFi.getMode() != WIFI_MODE_APSTA){
            WiFi.mode(WIFI_MODE_APSTA);         // Enable internal AP if station connection is lost
            LOG(println, F("UI WiFi: Switch to AP-Station mode"));
        }
        if(_cb_STADisconnected)
            _cb_STADisconnected();        // execule callback

        embuischedw.once(WIFI_RECONNECT_TIMER, [this](){ WiFi.begin(); } ); // esp32 doesn't auto-reconnects, so reschedule it
        break;
    default:
        break;
    }
    // handle network events for timelib
    timeProcessor.WiFiEvent(event, info);
}
#endif  //ESP32

void EmbUI::wifi_init(){
    String hn = param(FPSTR(P_hostname));
    String appwd = param(FPSTR(P_APpwd));

    if (!hn.length())
        var(FPSTR(P_hostname), String(__IDPREFIX) + mc, true);

    #ifdef ESP8266
        WiFi.hostname(hn);
    #elif defined ESP32
        WiFi.setHostname(hn.c_str());
        //WiFi.softAPsetHostname(hn);
    #endif

    if (appwd.length()<WIFI_PSK_MIN_LENGTH)
        appwd = "";

    LOG(printf_P, PSTR("UI WiFi: set AP params to SSID:%s, pwd:%s\n"), hn.c_str(), appwd.c_str());
    WiFi.softAP(hn.c_str(), appwd.c_str());

    String apmode = param(FPSTR(P_APonly));

    LOG(print, F("UI WiFi: start in "));
    if (apmode == FPSTR(P_true)){
        LOG(println, F("AP-only mode"));
        WiFi.mode(WIFI_AP);
    } else {
        LOG(println, F("AP/STA mode"));
        WiFi.mode(WIFI_AP_STA);
        WiFi.begin();   // use internaly stored last known credentials for connection
        LOG(println, F("UI WiFi reconecting..."));
    }
}

void EmbUI::wifi_connect(const char *ssid, const char *pwd)
{
    if (ssid) {
        WiFi.begin(ssid, pwd);
        LOG(printf_P, PSTR("UI WiFi: client connecting to SSID:%s, pwd:%s\n"), ssid, pwd ? pwd : "");
    } else {
        WiFi.begin();
    }
}

void EmbUI::wifi_setmode(WiFiMode_t mode){
    LOG(printf_P, PSTR("UI WiFi: set mode: %d\n"), mode);
    WiFi.mode(mode);
}

// TODO: adopt esp32 lib
#ifdef ESP8266
void EmbUI::setup_mDns(){
    /*use mdns for host name resolution*/
    char tmpbuf[32]; // Используем ap_ssid если задан, иначе конструируем вручную
    memset(tmpbuf,0,sizeof(tmpbuf));
    if(param(F("ap_ssid")).length()>0){
        strncpy_P(tmpbuf,param(F("ap_ssid")).c_str(),sizeof(tmpbuf)-1);
    }
    else
        sprintf_P(tmpbuf,PSTR("%s%s"),(char*)__IDPREFIX, mc);
    if (!MDNS.begin(tmpbuf, WiFi.softAPIP())) {
        Serial.println(F("Error setting up MDNS responder!"));
    } else {
        MDNS.addService(F("http"), F("tcp"), 80);
        MDNS.addService(F("ftp"), F("tcp"), 21);
        MDNS.addService(F("txt"), F("udp"), 4243);
        LOG(printf_P, PSTR("mDNS responder started: %s.local\n"),tmpbuf);
    }
}
#endif

/**
 * формирует chipid из MAC-адреса вида aabbccddeeff
 */
void EmbUI::getAPmac(){
    if(*mc) return;
    uint8_t _mac[6];

    #ifdef ESP8266
        wifi_get_macaddr(SOFTAP_IF, _mac);
    #elif defined ESP32
        WiFi.softAPmacAddress(_mac);
    #endif

    sprintf_P(mc, PSTR("%02X%02X%02X%02X%02X%02X"), _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
}