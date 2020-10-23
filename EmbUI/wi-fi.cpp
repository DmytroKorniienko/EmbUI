// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

//bool _wifi_connected = false;
#include "EmbUI.h"
#include "user_interface.h"
#include "wi-fi.h"
#include "timeProcessor.h"

#ifdef ESP8266
void EmbUI::onSTAConnected(WiFiEventStationModeConnected ipInfo)
{
    LOG(printf_P, PSTR("UI WiFi: connected to %s\r\n"), ipInfo.ssid.c_str());
}

void EmbUI::onSTAGotIP(WiFiEventStationModeGotIP ipInfo)
{
    wifi_setmode(WIFI_STA);            // Shutdown internal Access Point
    sysData.wifi_sta = true;
    embuischedw.detach();
    LOG(printf_P, PSTR("WiFi: Got IP: %s\r\n"), ipInfo.ip.toString().c_str());
    setup_mDns();
    timeProcessor.onSTAGotIP(ipInfo);
}

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
        Serial.printf_P(PSTR("mDNS responder started: %s.local\n"),tmpbuf);
    }
}

void EmbUI::onSTADisconnected(WiFiEventStationModeDisconnected event_info)
{
    if (embuischedw.active() && (WiFi.getMode()==WIFI_AP || WiFi.getMode()==WIFI_AP_STA || !sysData.wifi_sta))
        return;
    
    LOG(printf_P, PSTR("UI WiFi: Disconnected from SSID: %s, reason: %d\n"), event_info.ssid.c_str(), event_info.reason);
    sysData.wifi_sta = false;

    embuischedw.once_scheduled(WIFI_CONNECT_TIMEOUT, [this](){
        sysData.wifi_sta = false;
        LOG(println, F("UI WiFi: enabling internal AP"));
        wifi_setmode(WIFI_AP);  // Enable internal AP if station connection is lost
        embuischedw.once_scheduled(WIFI_RECONNECT_TIMER, [this](){wifi_setmode(WIFI_AP_STA); WiFi.begin(); setup_mDns();} );
    } );

    timeProcessor.onSTADisconnected(event_info);
}
#else
// need to test it under ESP32 (might not need any scheduler to handle both Client and AP at the same time)
void WiFiEvent(WiFiEvent_t event, system_event_info_t info)
{
    switch (event)
    {
    case SYSTEM_EVENT_STA_START:
        LOG(println, F("Station Mode Started"));
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        WiFi.mode(WIFI_STA);            // Shutdown internal Access Point
        LOG(printf_P, PSTR("Connected to: %s, got IP: %s\n", WiFi.SSID(), WiFi.localIP());
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        LOG(printf_P, PSTR("Disconnected from SSID: %s, reason: %d\n"), event_info.ssid.c_str(), event_info.reason);
        if(wifi_get_opmode() != WIFI_AP_STA){
            WiFi.mode(WIFI_AP_STA);         // Enable internal AP if station connection is lost
            LOG(println, F("Enabling internal AP"));
        }
        break;
    default:
        break;
    }
}
#endif

void EmbUI::wifi_connect(const char *ssid, const char *pwd)
{
    String hn = param(FPSTR(P_hostname));
    String appwd = param(FPSTR(P_APpwd));

    if (!hn.length())
        var(FPSTR(P_hostname), String(__IDPREFIX) + mc, true);
    WiFi.hostname(hn);

    if (appwd.length()<WIFI_PSK_MIN_LENGTH)
        appwd = "";

    LOG(printf_P, PSTR("WiFi: set AP params to SSID:%s, pwd:%s\n"), hn.c_str(), appwd.c_str());
    WiFi.softAP(hn.c_str(), appwd.c_str());

    String apmode = param(FPSTR(P_APonly));

    LOG(print, F("WiFi: start in "));
    if (apmode == FPSTR(P_true)){
        LOG(println, F("AP-only mode"));
        wifi_setmode(WIFI_AP);

    } else {
        LOG(println, F("AP/STA mode"));
        wifi_setmode(WIFI_AP_STA);

        if (ssid) {
            WiFi.begin(ssid, pwd);
            LOG(printf_P, PSTR("WiFi: client connecting to SSID:%s, pwd:%s\n"), ssid, pwd ? pwd : "");
        } else {
            WiFi.begin();   // use internaly stored last known credentials for connection
            LOG(println, F("WiFi reconecting..."));
        }
    }
}

void EmbUI::wifi_setmode(WiFiMode mode){
    LOG(printf_P, PSTR("WiFi: set mode: %d\n"), mode);
    WiFi.mode(mode);
}

/**
 * формирует chipid из MAC-адреса вида aabbccddeeff
 */
void EmbUI::getAPmac(){
    if(*mc) return;
/*
    #ifdef ESP32
    WiFi.mode(WIFI_MODE_AP);
    #else
    WiFi.mode(WIFI_AP);
    #endif
    String _mac(WiFi.softAPmacAddress());
    _mac.replace(F(":"), F(""));
    strncpy(mc, _mac.c_str(), sizeof(mc)-1);
*/
    uint8_t _mac[6];
    wifi_get_macaddr(SOFTAP_IF, _mac);

    sprintf_P(mc, PSTR("%02X%02X%02X%02X%02X%02X"), _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
}