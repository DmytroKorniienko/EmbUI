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
    LOG(printf_P, PSTR("UI WiFi: STA connected - SSID:'%s'"), ipInfo.ssid.c_str());
    if(_cb_STAConnected)
        _cb_STAConnected();        // execule callback
}

void EmbUI::onSTAGotIP(WiFiEventStationModeGotIP ipInfo)
{
    sysData.wifi_sta = true;
    embuischedw.disable();
    LOG(printf_P, PSTR(", IP: %s\n"), ipInfo.ip.toString().c_str());
    wifi_setmode(WIFI_STA);            // Shutdown internal Access Point
    timeProcessor.onSTAGotIP(ipInfo);
    if(_cb_STAGotIP)
        _cb_STAGotIP();        // execule callback

    setup_mDns();
}

void EmbUI::onSTADisconnected(WiFiEventStationModeDisconnected event_info)
{
    LOG(printf_P, PSTR("UI WiFi: Disconnected from SSID: %s, reason: %d\n"), event_info.ssid.c_str(), event_info.reason);
    sysData.wifi_sta = false;       // to be removed and replaced with API-method

    if (embuischedw.isEnabled())
        return;

    /*
      esp8266 сильно тормозит в комбинированном режиме AP-STA при постоянных попытках реконнекта, WEB-интерфейс становится
      неотзывчивым, сложно изменять настройки.
      В качестве решения переключаем контроллер в режим AP-only после WIFI_CONNECT_TIMEOUT таймаута на попытку переподключения.
      Далее делаем периодические попытки переподключений каждые WIFI_RECONNECT_TIMER секунд
    */
    embuischedw.set(WIFI_CONNECT_TIMEOUT * TASK_SECOND, TASK_ONCE, [this](){
        LOG(println, F("UI WiFi: switching to internal AP"));
        wifi_setmode(WIFI_AP);
        embuischedw.set(WIFI_RECONNECT_TIMER * TASK_SECOND, TASK_ONCE, [this](){ embuischedw.disable(); wifi_setmode(WIFI_AP_STA); WiFi.begin();} );
        embuischedw.restartDelayed();
    } );

    embuischedw.restartDelayed();

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
        WiFi.mode(WIFI_STA);            // Shutdown internal Access Point

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

        if(WiFi.getMode() != WIFI_MODE_STA){    // Switch to STA only mode once IP obtained
            WiFi.mode(WIFI_MODE_STA);
            LOG(println, F("UI WiFi: switch to STA mode"));
        }

        embuischedw.disable();
        sysData.wifi_sta = true;
        setup_mDns();
        if(_cb_STAGotIP)
            _cb_STAGotIP();        // execule callback
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        LOG(printf_P, PSTR("UI WiFi: Disconnected, reason: %d\n"), info.disconnected.reason);
        // https://github.com/espressif/arduino-esp32/blob/master/tools/sdk/include/esp32/esp_wifi_types.h
        if(WiFi.getMode() != WIFI_MODE_APSTA && !embuischedw.isEnabled()){
            LOG(println, PSTR("UI WiFi: Reconnect attempt"));
            embuischedw.set(WIFI_BEGIN_DELAY* TASK_SECOND, TASK_ONCE, [this](){ WiFi.mode(WIFI_MODE_APSTA);
                                                        LOG(println, F("UI WiFi: Switch to AP-Station mode"));
                                                        embuischedw.disable();} );
            embuischedw.restartDelayed();
        }

        sysData.wifi_sta = false;
        if(_cb_STADisconnected)
            _cb_STADisconnected();        // execule callback
        break;

    default:
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
        hn = String(__IDPREFIX) + mc;
        var(FPSTR(P_hostname), hn, true);
    }

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
    #ifdef ESP8266
        LOG(println, F("AP/STA mode"));
        WiFi.mode(WIFI_AP_STA);     // we start in combined STA mode, than disable AP once client get's IP address
    #elif defined ESP32
        LOG(println, F("STA mode"));
        WiFi.mode(WIFI_STA);       // we start in STA mode, esp32 can't set client's hotname in ap/sta
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
            embuischedw.set(WIFI_BEGIN_DELAY * TASK_SECOND, TASK_ONCE, [this](){ WiFi.mode(WIFI_MODE_APSTA);
                                                        LOG(println, F("UI WiFi: Switch to AP-Station mode"));
                                                        embuischedw.disable();} );
            embuischedw.restartDelayed();
        }

	    if (!WiFi.setHostname(hn.c_str()))
            LOG(println, F("UI WiFi: Failed to set hostname :("));
    #endif
        LOG(println, F("UI WiFi: STA reconecting..."));
    }
}

void EmbUI::wifi_connect(const char *ssid, const char *pwd)
{
    if (ssid){
        String _ssid(ssid); String _pwd(pwd);   // I need objects to pass it to the lambda
        embuischedw.set(WIFI_BEGIN_DELAY * TASK_SECOND, TASK_ONCE, [_ssid, _pwd, this](){
                    LOG(printf_P, PSTR("UI WiFi: client connecting to SSID:%s, pwd:%s\n"), _ssid.c_str(), _pwd.c_str());
                    #ifdef ESP32
                        WiFi.disconnect();
                	    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
                    #endif
                    WiFi.begin(_ssid.c_str(), _pwd.c_str());
	                embuischedw.disable();
            });
    } else {
        embuischedw.set(WIFI_BEGIN_DELAY * TASK_SECOND, TASK_ONCE, [this](){ WiFi.begin(); embuischedw.disable();} );
    }
    embuischedw.restartDelayed();
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
 * формирует chipid из MAC-адреса вида 'ddeeff'
 */
void EmbUI::getAPmac(){
    if(*mc) return;

    uint8_t _mac[6];

    #ifdef ESP32
        if(WiFi.getMode() == WIFI_MODE_NULL)
            WiFi.mode(WIFI_MODE_AP);
    #endif
    WiFi.softAPmacAddress(_mac);

    LOG(printf_P,PSTR("UI MAC ID:%02X%02X%02X\n"), _mac[3], _mac[4], _mac[5]);
    sprintf_P(mc, PSTR("%02X%02X%02X"), _mac[3], _mac[4], _mac[5]);
}
