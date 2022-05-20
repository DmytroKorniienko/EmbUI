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
    LOG(printf_P, PSTR("UI WiFi: STA connected - SSID:'%s'\n"), ipInfo.ssid.c_str());
    if(_cb_STAConnected)
        _cb_STAConnected();        // execule callback
}

void EmbUI::onSTAGotIP(WiFiEventStationModeGotIP ipInfo)
{
    sysData.wifi_sta = true;
    LOG(printf_P, PSTR("UI WiFi: IP: %s\n"), ipInfo.ip.toString().c_str());
    if(EmbUI::GetInstance()->param(FPSTR(P_WIFIMODE))!="2")
        WiFi.mode(WIFI_STA);            // Shutdown internal Access Point
    timeProcessor.onSTAGotIP(ipInfo);
    if(_cb_STAGotIP)
        _cb_STAGotIP();        // execule callback

    setup_mDns();
}

void EmbUI::onSTADisconnected(WiFiEventStationModeDisconnected event_info)
{
    LOG(printf_P, PSTR("UI WiFi: Disconnected from SSID: %s, reason: %d\n"), event_info.ssid.c_str(), event_info.reason);
    sysData.wifi_sta = false;       // to be removed and replaced with API-method

    if(param(FPSTR(P_WIFIMODE)) == "1")
        return;

    if (embuischedw)
        return;

    /*
      esp8266 сильно тормозит в комбинированном режиме AP-STA при постоянных попытках реконнекта, WEB-интерфейс становится
      неотзывчивым, сложно изменять настройки.
      В качестве решения переключаем контроллер в режим AP-only после EMBUI_WIFI_CONNECT_TIMEOUT таймаута на попытку переподключения.
      Далее делаем периодические попытки переподключений каждые EMBUI_WIFI_RECONNECT_TIMER секунд
    */
    if(WiFi.getMode() != WIFI_AP){
        LOG(println, F("UI WiFi: switching to internal AP"));
        wifi_setmode(WIFI_AP);
    }
    #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
        HeapSelectIram ephemeral;
    #endif
    embuischedw = new Task(EMBUI_WIFI_RECONNECT_TIMER * TASK_SECOND, TASK_ONCE,
            [this](){ wifi_setmode(WIFI_AP_STA); WiFi.begin(); TASK_RECYCLE; embuischedw = nullptr; },
            &ts, false
        );
    embuischedw->enableDelayed();

    timeProcessor.onSTADisconnected(event_info);
    if(_cb_STADisconnected)
        _cb_STADisconnected();        // execute callback
}

void EmbUI::onWiFiMode(WiFiEventModeChange event_info){
    if(WiFi.getMode() == WIFI_AP){
        setup_mDns();
    }
}
#endif  //ESP8266

#ifdef ESP32
void EmbUI::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    switch (event){
    case SYSTEM_EVENT_AP_START:
        LOG(println, F("UI WiFi: Access-point started"));
        setup_mDns();
        break;

    case SYSTEM_EVENT_STA_CONNECTED:
        LOG(println, F("UI WiFi: STA connected"));

        if(_cb_STAConnected)
            _cb_STAConnected();        // execule callback
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
    	/* this is a weird hack to mitigate DHCP-client hostname issue
	     * https://github.com/espressif/arduino-esp32/issues/2537
         * we use some IDF functions to restart dhcp-client, that has been disabled before STA connect
	    */
	    tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA);
	    tcpip_adapter_ip_info_t iface;
	    tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &iface);
        if(!iface.ip.addr){
            LOG(println, F("UI WiFi: DHCP discover..."));
	        return;
    	}

        LOG(printf_P, PSTR("SSID:'%s', IP: "), WiFi.SSID().c_str());  // IPAddress(info.got_ip.ip_info.ip.addr)
        LOG(println, IPAddress(iface.ip.addr));

        if(WiFi.getMode() != WIFI_STA && EmbUI::GetInstance()->param(FPSTR(P_WIFIMODE))!="2"){    // Switch to STA only mode once IP obtained
            WiFi.mode(WIFI_STA);
            LOG(println, F("UI WiFi: switch to STA mode"));
        }

        sysData.wifi_sta = true;
        setup_mDns();
        if(_cb_STAGotIP)
            _cb_STAGotIP();        // execule callback
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        #if ARDUINO > 10805
            LOG(printf_P, PSTR("UI WiFi: Disconnected, reason: %d\n"), info.wifi_sta_disconnected.reason);
        #else
            LOG(printf_P, PSTR("UI WiFi: Disconnected, reason: %d\n"), info.disconnected.reason);
        #endif
        // https://github.com/espressif/arduino-esp32/blob/master/tools/sdk/include/esp32/esp_wifi_types.h
        if(WiFi.getMode() != WIFI_AP_STA){
            LOG(println, F("UI WiFi: Reconnect attempt"));
            LOG(println, F("UI WiFi: Switch to AP-Station mode"));
        }

        embuischedw = new Task(EMBUI_WIFI_RECONNECT_TIMER * TASK_SECOND, TASK_ONCE,
                [this](){ wifi_setmode(WIFI_AP_STA); WiFi.begin(); TASK_RECYCLE; embuischedw = nullptr; },
                &ts, false
            );
        embuischedw->enableDelayed();

        sysData.wifi_sta = false;
        if(_cb_STADisconnected)
            _cb_STADisconnected();        // execule callback
        break;
    case SYSTEM_EVENT_SCAN_DONE:
        //BasicUI::scan_complete(info.scan_done.number);
        #if ARDUINO > 10805
            EmbUI::GetInstance()->pf_wifiscan(info.wifi_scan_done.number);
        #else
            EmbUI::GetInstance()->pf_wifiscan(info.scan_done.number);
        #endif
        break;
    default:
        LOG(printf_P, PSTR("UI WiFi: Unhandled event: %d\n"), event);
        break;
    }
    timeProcessor.WiFiEvent(event, info);    // handle network events for timelib
}
#endif  //ESP32

void EmbUI::wifi_init(){
    String hn = param(FPSTR(P_hostname));
    String appwd = param(FPSTR(P_APpwd));
    getAPmac();
    if (!hn.length()){
        hn = String(F(TOSTRING(EMBUI_IDPREFIX))) + String(mc);
        var(FPSTR(P_hostname), hn, true);
    }

    if (appwd.length()<EMBUI_WIFI_PSK_MIN_LENGTH)
        appwd = "";

    LOG(printf_P, PSTR("UI WiFi: set AP params to SSID:%s, pwd:%s\n"), hn.c_str(), appwd.c_str());
    WiFi.softAP(hn.c_str(), appwd.c_str());

    String apmode = param(FPSTR(P_WIFIMODE));

    LOG(print, F("UI WiFi: start in "));
    if (apmode == "1"){
        LOG(println, F("AP-only mode"));
        WiFi.mode(WIFI_AP);
    } else if(apmode != "1"){ // 0 & 2
    #ifdef ESP8266
        LOG(println, F("AP/STA mode"));
        WiFi.mode(WIFI_AP_STA);     // we start in combined STA mode, than disable AP once client get's IP address
    #elif defined ESP32
        //LOG(println, F("STA mode"));
        //WiFi.mode(WIFI_STA);       // we start in STA mode, esp32 can't set client's hostname in ap/sta
        LOG(println, F("AP/STA mode"));
        WiFi.mode(WIFI_AP_STA);     // we start in combined STA mode, than disable AP once client get's IP address
    #endif

    #ifdef ESP8266
        WiFi.hostname(hn);
        WiFi.begin();   // use internaly stored last known credentials for connection
    #elif defined ESP32
    	/* this is a weird hack to mitigate DHCP hostname issue
	     * order of initialization does matter, pls keep it like this till fixed in upstream
	     * https://github.com/espressif/arduino-esp32/issues/2537
	     */
	    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
        // use internaly stored last known credentials for connection
        if ( WiFi.begin() == WL_CONNECT_FAILED ){
            embuischedw = new Task(EMBUI_WIFI_BEGIN_DELAY * TASK_SECOND, TASK_ONCE,
                    [this](){ wifi_setmode(WIFI_AP_STA); LOG(println, F("UI WiFi: Switch to AP-Station mode")); WiFi.begin(); TASK_RECYCLE; embuischedw = nullptr; },
                    &ts, false
                );
            embuischedw->enableDelayed();
        }

	    if (!WiFi.setHostname(hn.c_str()))
            LOG(println, F("UI WiFi: Failed to set hostname :("));
    #endif
        LOG(println, F("UI WiFi: STA reconecting..."));
    }
}

void EmbUI::wifi_connect(const String &ssid, const String &pwd)
{
    if ((!ssid.isEmpty() && !pwd.isEmpty()) || (!ssid.isEmpty() && WiFi.SSID()!=String(ssid))){
        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        String _ssid(ssid); String _pwd(pwd);   // I need objects to pass it to the lambda
        embuischedw = new Task(EMBUI_WIFI_BEGIN_DELAY * TASK_SECOND, TASK_ONCE, [_ssid, _pwd, this](){
                    LOG(printf_P, PSTR("UI WiFi: client connecting to SSID:%s, pwd:%s\n"), _ssid.c_str(), _pwd.c_str());
                    #ifdef ESP32
                        //WiFi.disconnect();
                	    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
                    #else
                        WiFi.persistent(true); // SDK3.0.0+ ESP8266 == false by default, this cause issue of first connect
                        LOG(printf_P,PSTR("UI WiFi: WiFi.getAutoConnect()=%d, WiFi.getAutoReconnect()=%d, WiFi.getPersistent()=%d\n"), WiFi.getAutoConnect(), WiFi.getAutoReconnect(), WiFi.getPersistent());
                        //WiFi.setAutoReconnect(true);
                        //WiFi.setAutoConnect(true);
                    #endif
                    WiFi.begin(_ssid.c_str(), _pwd.c_str());
                    TASK_RECYCLE; embuischedw = nullptr;
            }, &ts, false);
    } else {
        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        embuischedw = new Task(EMBUI_WIFI_BEGIN_DELAY * TASK_SECOND, TASK_ONCE,
                [this](){ WiFi.begin(); TASK_RECYCLE; embuischedw = nullptr; },
                &ts, false
            );
    }
    embuischedw->enableDelayed();
}


void EmbUI::wifi_setmode(WiFiMode_t mode){
    LOG(printf_P, PSTR("UI WiFi: set mode: %d\n"), mode);
    WiFi.mode(mode);
}

/*use mdns for host name resolution*/
void EmbUI::setup_mDns(){
    String hostname = param(FPSTR(P_hostname));

#ifdef ESP8266
    if (MDNS.isRunning())
#endif
        MDNS.end();

    if (!MDNS.begin(hostname.c_str())){
        LOG(println, F("UI mDNS: Error setting up responder!"));
        MDNS.end();
        return;
    }

    MDNS.addService(F("http"), F("tcp"), 80);
    //MDNS.addService(F("ftp"), F("tcp"), 21);
    MDNS.addService(F("txt"), F("udp"), 4243);
    LOG(printf_P, PSTR("UI mDNS: responder started: %s.local\n"),hostname.c_str());
}

/**
 * формирует chipid из MAC-адреса вида 'aabbccddeeff'
 */
void EmbUI::getAPmac(){
    if(*mc) return;

    uint8_t _mac[6];

    #ifdef ESP32
        if(WiFi.getMode() == WIFI_MODE_NULL)
            WiFi.mode(WIFI_MODE_AP);
    #endif
    WiFi.softAPmacAddress(_mac);

    sprintf_P(mc, PSTR("%02X%02X%02X%02X%02X%02X"), _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
    LOG(printf_P,PSTR("UI MAC ID: %s\n"), mc);
}
