// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "timeProcessor.h"
#include "ts.h"
#include <ArduinoJson.h>

#ifdef ESP8266
#include <coredecls.h>                 // settimeofday_cb()
#include <TZ.h>                        // TZ declarations https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
#include <sntp.h>
#include <ESP8266HTTPClient.h>

#ifdef __NEWLIB__ 
#if __NEWLIB__ >= 4
    extern "C" {
        #include <sys/_tz_structs.h>
    };
#endif
#endif
#endif

#ifdef ESP32
 #include <time.h>
 #include <lwip/apps/sntp.h>
 #include <HTTPClient.h>
 #include "TZ32.h"
#endif

static const char P_LOC[] PROGMEM = "LOC";

#ifndef ESP8266
namespace RTC_MEM_32 {
    RTC_DATA_ATTR RTC_DATA rtcTime;
};

using namespace RTC_MEM_32;
#endif

unsigned long RTC_Worker(unsigned long _storage=0){
#ifdef ESP8266
    RTC_DATA rtcTime;
    uint32_t rtc_time = system_get_rtc_time();
    if(rtc_time<500000){
        rtcTime.timeBase = rtc_time;
        rtcTime.timeAcc = 0;
        rtcTime.storage = 0;
        //LOG(printf_P, PSTR("%d - %d - %lld - %d\n"), rtc_time, rtcTime.timeBase, rtcTime.timeAcc, (rtcTime.timeAcc / 1000000) / 1000);
        //ESP.rtcUserMemoryWrite(128-sizeof(RTC_DATA), (uint32_t*)&rtcTime, sizeof(RTC_DATA));
        system_rtc_mem_write(192-sizeof(RTC_DATA), &rtcTime, sizeof(RTC_DATA));
    } else {
        //ESP.rtcUserMemoryRead(128-sizeof(RTC_DATA), (uint32_t*)&rtcTime, sizeof(RTC_DATA));
        system_rtc_mem_read(192-sizeof(RTC_DATA), &rtcTime, sizeof(RTC_DATA));
        rtc_time = system_get_rtc_time();
        uint32_t cal = system_rtc_clock_cali_proc();
        rtcTime.timeAcc += ((uint64_t)(rtc_time - rtcTime.timeBase) * (((uint64_t)cal * 1000) >> 12));
        //LOG(printf_P, PSTR("%d - %d - %lld - %d\n"), rtc_time, rtcTime.timeBase, rtcTime.timeAcc, (rtcTime.timeAcc / 1000000) / 1000);
        rtcTime.timeBase = rtc_time;
        if(_storage)
            rtcTime.storage = _storage - (rtcTime.timeAcc / 1000000) / 1000;
        //ESP.rtcUserMemoryWrite(128-sizeof(RTC_DATA), (uint32_t*)&rtcTime, sizeof(RTC_DATA));
        system_rtc_mem_write(192-sizeof(RTC_DATA), &rtcTime, sizeof(RTC_DATA));
    }
    LOG(printf_P, PSTR("TIME: RTC time = %d sec (%d)\n"), (uint32_t)(rtcTime.timeAcc / 1000000) / 1000, rtcTime.storage);
    return rtcTime.storage+(rtcTime.timeAcc / 1000000) / 1000;
#else
    struct timeval tv = { .tv_sec = 0, .tv_usec = 0 };   /* btw settimeofday() is helpfull here too*/
    // uint64_t sec, us;
    gettimeofday(&tv, NULL);
    // (sec) = tv.tv_sec;
    // (us) = tv.tv_usec;
    uint32_t rtc_time = tv.tv_sec;

    if(rtc_time<500000){
        rtcTime.timeBase = rtc_time;
        rtcTime.timeAcc = 0;
        rtcTime.storage = 0;
        //LOG(printf_P, PSTR("%d - %d - %lld\n"), rtc_time, rtcTime.timeBase, rtcTime.timeAcc);
    } else {
        rtcTime.timeAcc += (uint64_t)(rtc_time - rtcTime.timeBase);
        //LOG(printf_P, PSTR("%d - %d - %lld\n"), rtc_time, rtcTime.timeBase, rtcTime.timeAcc);
        rtcTime.timeBase = rtc_time;
        if(_storage)
            rtcTime.storage = _storage - rtcTime.timeAcc;
    }
    LOG(printf_P, PSTR("TIME: RTC time = %lld sec (%ld)\n"), rtcTime.timeAcc, rtcTime.storage);
    return rtcTime.storage+rtcTime.timeAcc;
#endif
}

TimeProcessor::TimeProcessor()
{
    //configTzTime(); for esp32 https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-time.c

#ifdef ESP8266
    settimeofday_cb( [this]{ timeavailable();} );
#endif

/*
для ESP32 функциональный коллбек не поддерживается :( 
#ifdef ESP32
    sntp_set_time_sync_notification_cb(timeavailable);
#endif
*/
    configTzTime(EMBUI_DEF_TZONE, EMBUI_NTP1ADDRESS, EMBUI_NTP2ADDRESS);
    //LOG(printf_P, PSTR("TIME: Time Zone set to: %s\n"), EMBUI_DEF_TZONE);

    sntp_stop();    // отключаем ntp пока нет подключения к AP
}

String TimeProcessor::getFormattedShortTime()
{
    char buffer[6];
    sprintf_P(buffer,PSTR("%02u:%02u"), localtime(now())->tm_hour, localtime(now())->tm_min);
    return String(buffer);
}

/**
 * установка строки с текущей временной зоной в текстовом виде
 * влияет, на запрос через http-api за временем в конкретной зоне,
 * вместо автоопределения по ip
 */
void TimeProcessor::httpTimezone(const char *var){
  if (!var)
    return;
  tzone = var;
}

/**
 * по идее это функция установки времени из гуя.
 * Но похоже что выставляет она только часы и минуты, и то не очень понятно куда?
 * надо переделать под выставление даты/веремени из браузера (например) когда нормально заработает гуй
 */
void TimeProcessor::setTime(const char *timestr){
    String _str(timestr);
    setTime(_str);
}

void TimeProcessor::setTime(const String &timestr){
    //"YYYY-MM-DDThh:mm:ss"    [19]
    if (timestr.length()<EMBUI_DATETIME_STRLEN)
        return;

    struct tm t;
    tm *tm=&t;

    tm->tm_year = timestr.substring(0,4).toInt() - EMBUI_TM_BASE_YEAR;
    tm->tm_mon = timestr.substring(5,7).toInt()-1;
    tm->tm_mday = timestr.substring(8,10).toInt();
    tm->tm_hour= timestr.substring(11,13).toInt();
    tm->tm_min = timestr.substring(14,16).toInt();
    tm->tm_sec = timestr.substring(17,19).toInt();

    timeval tv = { mktime(tm), 0 };
    settimeofday(&tv, NULL);
    RTC_Worker(tv.tv_sec);
}


/**
 * установки системной временной зоны/правил сезонного времени.
 * по сути дублирует системную функцию setTZ, но работает сразу
 * со строкой из памяти, а не из PROGMEM
 * Может использоваться для задания настроек зоны/правил налету из
 * браузера/апи вместо статического задания Зоны на этапе компиляции
 * @param tz - указатель на строку в формате TZSET(3)
 * набор отформатированных строк зон под прогмем лежит тут
 * https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
 */
void TimeProcessor::tzsetup(const char* tz){
    // https://stackoverflow.com/questions/56412864/esp8266-timezone-issues
    if (!tz || !*tz)
        return;

    /*
     * newlib has issues with TZ strings with quoted <+-nn> names 
     * this has been fixed in https://github.com/esp8266/Arduino/pull/7702 for esp8266 (still not in stable as of Nov 16 2020)
     * but it also affects ESP32 and who knows when to expect a fix there
     * So let's fix such zones in-place untill core support for both platforms available
     */
    if (tz[0] == 0x3C){     // check if first char is '<'
      String _tz(tz);
      String _tzfix((char *)0);
      _tzfix.reserve(sizeof(tz)) ;
      _tzfix += FPSTR(P_LOC);
      if (_tz.indexOf('<',1) > 0){  // there might be two <> quotes
    	//LOG(print, "2nd pos: "); LOG(println, _tz.indexOf('<',1)); 
        _tzfix += _tz.substring(_tz.indexOf('>')+1, _tz.indexOf('<',1));
        _tzfix += FPSTR(P_LOC);
      }
      _tzfix += _tz.substring(_tz.lastIndexOf('>')+1, _tz.length());
      setenv("TZ", _tzfix.c_str(), 1/*overwrite*/);
      LOG(printf_P, PSTR("TIME: TZ fix applied: %s\n"), _tzfix.c_str());
    } else {
      setenv("TZ", tz, 1/*overwrite*/);
    }

    tzset();
    tzone = ""; // сбрасываем костыльную зону
    tpData.usehttpzone = false;  // запрещаем использование http
    LOG(printf_P, PSTR("TIME: TZSET rules changed from: %s to: %s\n"), EMBUI_DEF_TZONE, tz);

    unsigned long shift = RTC_Worker();
    //time_t _time = shift;
    //timeval tv = { _time, 0 };
    //settimeofday(&tv, NULL);
    String dt; getDateTimeString(dt, shift);
    LOG(printf_P,PSTR("TIME: After reboot time (%lu)-> %s\n"), (unsigned long)shift, dt.c_str());
}

/**
 * берем урл и записываем ответ в переданную строку
 * в случае если в коде ответа ошибка, обнуляем строку
 */ 
unsigned int TimeProcessor::getHttpData(String &payload, const String &url)
{
  WiFiClient client;
  HTTPClient http;
  LOG(printf_P, PSTR("TimeZone updating via HTTP: %s\n"), url.c_str());
  http.begin(client, url);

  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK){
    payload = http.getString(); 
  } else {
    LOG(printf_P, PSTR("Time HTTPCode=%d\n"), httpCode);
  }
  http.end();
  return payload.length();
}

void TimeProcessor::getTimeHTTP()
{
    if (!tpData.usehttpzone)
        return;     // выходим если не выставлено разрешение на использование http

    String result((char *)0);
    result.reserve(EMBUI_TIMEAPI_BUFSIZE);
    if(tzone.length()){
        String url(FPSTR(PG_timeapi_tz_url));
        url+=tzone;
        getHttpData(result, url);
    }

    if(!result.length()){
        String url(FPSTR(PG_timeapi_ip_url));
        if(!getHttpData(result, url))
            return;
    }

    LOG(println, result);
    DynamicJsonDocument doc(EMBUI_TIMEAPI_BUFSIZE);
    DeserializationError error = deserializeJson(doc, result);
    result="";

    if (error) {
        LOG(print, F("Time deserializeJson error: "));
        LOG(println, error.code());
        return;
    }

    int raw_offset, dst_offset = 0;

    raw_offset=doc[F("raw_offset")];    // по сути ничего кроме текущего смещения от UTC от сервиса не нужно
                                        // правила перехода сезонного времени в формате, воспринимаемом системной
                                        // либой он не выдает, нужно писать внешний парсер. Мнемонические определения
                                        // слишком объемные для контроллера чтобы держать и обрабатывать их на лету.
                                        // Вероятно проще будет их запихать в js веб-интерфейса
    dst_offset=doc[F("dst_offset")];

    // Save mnemonic time-zone (do not know why :) )
    if (!tzone.length()) {
        const char *tz = doc[F("timezone")];
        tzone+=tz;
    }
    LOG(printf_P, PSTR("HTTP TimeZone: %s, offset: %d, dst offset: %d\n"), tzone.c_str(), raw_offset, dst_offset);

    setOffset(raw_offset+dst_offset);

    if (doc[F("dst_from")]!=nullptr){
        LOG(println, F("Zone has DST, rescheduling refresh"));
        httprefreshtimer();
    }
}

void TimeProcessor::httprefreshtimer(const uint32_t delay){
    if (tpData.usehttpzone && _httpTask){
        _httpTask->disable();
        return;
    }

    time_t timer;

    if (delay){
        timer = delay;
    } else {
        struct tm t;
        tm *tm=&t;
        localtime_r(now(), tm);

        tm->tm_mday++;                  // выставляем "завтра"
        tm->tm_hour= EMBUI_HTTP_REFRESH_HRS;
        tm->tm_min = EMBUI_HTTP_REFRESH_MIN;

        timer = (mktime(tm) - getUnixTime())% EMBUI_DAYSECONDS;

        LOG(printf_P, PSTR("Schedule TZ refresh in %ld\n"), timer);
    }

    if(!_httpTask){
        #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
            HeapSelectIram ephemeral;
        #endif
        _httpTask = new Task(timer * TASK_SECOND, TASK_ONCE, nullptr, &ts, false, nullptr, [this](){getTimeHTTP(); TASK_RECYCLE; _httpTask=nullptr;});
    }
    _httpTask->restartDelayed();
}

void TimeProcessor::ntpReSync(){
    if(!sntpIsSynced() || !tpData.isSynced){
#ifndef ESP32
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
#endif
        if(!_ntpTask){
            #if defined(PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED) && defined(EMBUI_USE_SECHEAP)
                HeapSelectIram ephemeral;
            #endif
            _ntpTask = new Task(TASK_SECOND*10, TASK_ONCE, nullptr, &ts, false, nullptr, [this](){
                if((!sntpIsSynced() || !tpData.isSynced) && tpData.ntpcnt){
                    const char *to;
                    switch(tpData.ntpcnt){
                        case 1: to = EMBUI_NTP1ADDRESS; break;
                        case 2: to = EMBUI_NTP2ADDRESS; break;
                        case 3: to = ntp2.c_str(); break;
                        default: to = EMBUI_NTP1ADDRESS; break;
                    }
                    LOG(printf_P, PSTR("NTP: switching NTP[%d] server from %s to %s\n"), tpData.ntpcnt, String(sntp_getservername(0)).c_str(), to); // странное преобразование, но почему-то без него бывают ребуты...
                    sntp_stop();
                    sntp_setservername(0, (char *)to);
                    sntp_init();
                    tpData.ntpcnt++; tpData.ntpcnt%=4;
                    ts.getCurrentTask()->restartDelayed(TASK_SECOND*10);
                    return;
                } else {
                    if(!tpData.ntpcnt){
                        unsigned long shift = RTC_Worker();
                        time_t _time = shift;
                        timeval tv = { _time, 0 };
                        settimeofday(&tv, NULL);
                        String dt; getDateTimeString(dt);
                        LOG(printf_P,PSTR("TIME: Get time from RTC (%lu)-> %s\n"), (unsigned long)shift, dt.c_str());
                    }
                    _ntpTask = nullptr;
                    TASK_RECYCLE;
                }
            });
            _ntpTask->enableDelayed();
        } else {
            _ntpTask->restartDelayed();
        }
    }
}

/**
 * обратный вызов при подключении к WiFi точке доступа
 * запускает синхронизацию времени
 */
#ifdef ESP8266
void TimeProcessor::onSTAGotIP(const WiFiEventStationModeGotIP ipInfo)
{
    sntp_init();
    ntpReSync();
    if(tpData.usehttpzone){
        // отложенный запрос смещения зоны через http-сервис
        httprefreshtimer(EMBUI_HTTPSYNC_DELAY);
    }
}

void TimeProcessor::onSTADisconnected(const WiFiEventStationModeDisconnected event_info)
{
    sntp_stop();
    if(_httpTask && tpData.usehttpzone){
        _httpTask->disable();
    }
}
#endif  //ESP8266

#ifdef ESP32
void TimeProcessor::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info){
    switch (event)
    {
    case SYSTEM_EVENT_STA_GOT_IP:
        sntp_init();
        ntpReSync();
        if(tpData.usehttpzone){
            // отложенный запрос смещения зоны через http-сервис
            httprefreshtimer(EMBUI_HTTPSYNC_DELAY);
        }
        LOG(println, F("UI TIME: Starting sntp sync"));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        sntp_stop();
        if(_httpTask && tpData.usehttpzone){
            _httpTask->disable();
        }
        break;
    default:
        break;
    }
}
#endif  //ESP32


void TimeProcessor::timeavailable(){
    LOG(println, F("UI TIME: Time adjusted"));
    if(sntpIsSynced()){
        unsigned long shift;
        time((time_t *)&shift);
        RTC_Worker(shift);
        LOG(printf_P, PSTR("NTP: time synced from %s (%lu)\n"), String(sntp_getservername(0)).c_str(), (unsigned long)shift); // странное преобразование, но почему-то без него бывают ребуты...
        tpData.isSynced = true;
        if(_timecallback)
            _timecallback();
    }
}

/**
 * функция допечатывает в переданную строку передайнный таймстамп даты/времени в формате "9999-99-99T99:99"
 * @param _tstamp - преобразовать заданный таймстамп, если не задан используется текущее локальное время
 */
void TimeProcessor::getDateTimeString(String &buf, const time_t _tstamp){
  char tmpBuf[EMBUI_DATETIME_STRLEN];
  const tm* tm = localtime(  _tstamp ? &_tstamp : now());
  sprintf_P(tmpBuf,PSTR("%04u-%02u-%02uT%02u:%02u"), tm->tm_year + EMBUI_TM_BASE_YEAR, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
  buf.concat(tmpBuf);
}

/**
 * установка текущего смещения от UTC в секундах
 */
void TimeProcessor::setOffset(const int val){
    LOG(printf_P, PSTR("UI Time: Set time zone offset to: %d\n"), val);

    #ifdef ESP8266
        sntp_set_timezone_in_seconds(val);
    #elif defined ESP32
        //setTimeZone((long)val, 0);    // this breaks linker in some weird way
        //configTime((long)val, 0, ntp0.c_str(), ntp1.c_str(), "");
        configTime((long)val, 0, EMBUI_NTP1ADDRESS, EMBUI_NTP2ADDRESS, "");
    #endif

    // в правилах TZSET смещение имеет обратный знак (TZ-OffSet=UTC)
    // возможно это нужно будет учесть если задавать смещение для tz из правил (на будущее)
}

/**
 * Возвращает текущее смещение локального системного времени от UTC в секундах
 * с учетом часовой зоны и правил смены сезонного времени (если эти параметры были
 * корректно установленно ранее каким-либо методом)
 */
long int TimeProcessor::getOffset(){
    const tm* tm = localtime(now());
    auto tz = __gettzinfo();
    return *(tm->tm_isdst == 1 ? &tz->__tzrule[1].offset : &tz->__tzrule[0].offset);
}

void TimeProcessor::setcustomntp(const char* ntp){
    if (!ntp || !*ntp)
             return;

    this->ntp2 = ntp;
    sntp_setservername(EMBUI_CUSTOM_NTP_INDEX, (char *)this->ntp2.c_str());
    LOG(printf_P, PSTR("NTP: Set custom NTP[%d] to: %s\n"), EMBUI_CUSTOM_NTP_INDEX, this->ntp2.c_str());
    this->tpData.ntpcnt = EMBUI_CUSTOM_NTP_INDEX;

    // sntp_restart();
    ntpReSync();
}

void TimeProcessor::attach_callback(callback_function_t callback){
    _timecallback = std::move(callback);
}

bool TimeProcessor::sntpIsSynced()
{
    time_t now;
    tm *timeinfo;
    bool rc;

    time(&now);
    timeinfo = localtime(&now);

    //LOG(printf_P, PSTR("NTP: timeinfo->tm_year=%d\n"), timeinfo->tm_year);

    if (timeinfo->tm_year < (2000 - 1900))
    {
        rc = false;
    }
    else
    {
#ifdef ESP32
        tpData.isSynced = true;
#endif        
        rc = true;
    }
    return rc;
}