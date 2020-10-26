// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "timeProcessor.h"

#ifdef ESP8266
 #include <coredecls.h>                 // settimeofday_cb()
 #include <TZ.h>                        // TZ declarations https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
 #include <sntp.h>
 #include <ESP8266HTTPClient.h>
#endif

#ifdef ESP32
 #include <time.h>
 #include <lwip/apps/sntp.h>
 #include <HTTPClient.h>
 //extern "C" void setTimeZone(long offset, int daylight = 0);
#endif

#ifndef TZONE
  #include <ArduinoJson.h>
#endif

#define TZ_DEFAULT PSTR("GMT0")         // default Time-Zone

TimeProcessor::TimeProcessor()
{
    // moved to the embui wifi management
    // eGotIPHandler = WiFi.onStationModeGotIP(std::bind(&TimeProcessor::onSTAGotIP, this, std::placeholders::_1));
    // eDisconnectHandler = WiFi.onStationModeDisconnected(std::bind(&TimeProcessor::onSTADisconnected, this, std::placeholders::_1));
    //configTzTime(); for esp32 https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-time.c

#ifdef ESP8266
    settimeofday_cb(std::bind(&TimeProcessor::timeavailable, this));
#endif

    #ifdef TZONE
        #ifdef ESP8266
          configTime(TZONE, NTP1ADDRESS, NTP2ADDRESS);
        #elif defined ESP32
          configTzTime(TZONE, NTP1ADDRESS, NTP2ADDRESS);
        #endif
        LOG(print, F("TIME: Time Zone set to: "));      LOG(print, TZONE);
    #else
        #ifdef ESP8266
          configTime(TZ_DEFAULT, NTP1ADDRESS, NTP2ADDRESS);
        #elif defined ESP32
          configTzTime(TZ_DEFAULT, NTP1ADDRESS, NTP2ADDRESS);
        #endif
    #endif

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
    if (timestr.length()<DATETIME_STRLEN)
        return;

    struct tm t;
    tm *tm=&t;

    tm->tm_year = timestr.substring(0,4).toInt() - TM_BASE_YEAR;
    tm->tm_mon = timestr.substring(5,7).toInt()-1;
    tm->tm_mday = timestr.substring(8,10).toInt();
    tm->tm_hour= timestr.substring(11,13).toInt();
    tm->tm_min = timestr.substring(14,16).toInt();
    tm->tm_sec = timestr.substring(17,19).toInt();

    timeval tv = { mktime(tm), 0 };
    settimeofday(&tv, NULL);
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

    setenv("TZ", tz, 1/*overwrite*/);
    tzset();
    tzone = ""; // сбрасываем костыльную зону
    usehttpzone = false;  // запрещаем использование http
    LOG(printf_P, PSTR("TIME: TZSET rules changed to: %s\n"), tz);
}

#ifndef TZONE
/**
 * берем урл и записываем ответ в переданную строку
 * в случае если в коде ответа ошибка, обнуляем строку
 */ 
unsigned int TimeProcessor::getHttpData(String &payload, const String &url)
{
  WiFiClient client;
  HTTPClient http;
  LOG(println, F("TimeZone updating via HTTP..."));
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
    if (!usehttpzone)
        return;     // выходим если не выставлено разрешение на использование http

    String result((char *)0);
    result.reserve(TIMEAPI_BUFSIZE);
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
    DynamicJsonDocument doc(TIMEAPI_BUFSIZE);
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
    setOffset(raw_offset+dst_offset);

    if (!tzone.length()) {
        const char *tz = doc[F("timezone")];
        tzone+=tz;
    }
    LOG(printf_P, PSTR("HTTP TimeZone: %s, offset: %d, dst offset: %d\n"), tzone.c_str(), raw_offset, dst_offset);

    if (doc[F("dst_from")]!=nullptr){
        LOG(println, F("Zone has DST, rescheduling refresh"));
        httprefreshtimer();
    }
}

void TimeProcessor::httprefreshtimer(const uint32_t delay){
    if (!usehttpzone){
        _wrk.detach();
        return;     // выходим если не выставлено разрешение на использование http
    }

    time_t timer;

    if (delay){
        timer = delay;
    } else {
        struct tm t;
        tm *tm=&t;
        localtime_r(now(), tm);

        tm->tm_mday++;                  // выставляем "завтра"
        tm->tm_hour= HTTP_REFRESH_HRS;
        tm->tm_min = HTTP_REFRESH_MIN;

        timer = (mktime(tm) - getUnixTime())% DAYSECONDS;

        LOG(printf_P, PSTR("Schedule TZ refresh in %ld\n"), timer);
    }

    #ifdef ESP8266
        _wrk.once_scheduled(timer, std::bind(&TimeProcessor::getTimeHTTP, this));
    #elif defined ESP32
        _wrk.once((float)timer, std::bind(&TimeProcessor::getTimeHTTP, this));
    #endif

}
#endif

/**
 * обратный вызов при подключении к WiFi точке доступа
 * запускает синхронизацию времени
 */
#ifdef ESP8266
void TimeProcessor::onSTAGotIP(const WiFiEventStationModeGotIP ipInfo)
{
    sntp_init();
    #ifndef TZONE
        // отложенный запрос смещения зоны через http-сервис
        httprefreshtimer(HTTPSYNC_DELAY);
    #endif
}

void TimeProcessor::onSTADisconnected(const WiFiEventStationModeDisconnected event_info)
{
  sntp_stop();
  #ifndef TZONE
    _wrk.detach();
  #endif
}
#endif  //ESP8266

#ifdef ESP32
void TimeProcessor::WiFiEvent(WiFiEvent_t event, system_event_info_t info){
    switch (event)
    {
    case SYSTEM_EVENT_STA_GOT_IP:
        sntp_init();
        #ifndef TZONE
            // отложенный запрос смещения зоны через http-сервис
            httprefreshtimer(HTTPSYNC_DELAY);
        #endif
        LOG(println, F("UI TIME: Starting sntp sync"));
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        sntp_stop();
        #ifndef TZONE
            _wrk.detach();
        #endif
        break;
    default:
        break;
    }
}
#endif  //ESP32


void TimeProcessor::timeavailable(){
    LOG(println, F("UI TIME: Time adjusted"));
    isSynced = true;
    if(_timecallback)
        _timecallback();
}

/**
 * функция допечатывает в переданную строку передайнный таймстамп даты/времени в формате "9999-99-99T99:99"
 * @param _tstamp - преобразовать заданный таймстамп, если не задан используется текущее локальное время
 */
void TimeProcessor::getDateTimeString(String &buf, const time_t _tstamp){
  char tmpBuf[DATETIME_STRLEN];
  const tm* tm = localtime(  _tstamp ? &_tstamp : now());
  sprintf_P(tmpBuf,PSTR("%04u-%02u-%02uT%02u:%02u"), tm->tm_year + TM_BASE_YEAR, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min);
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
        configTime((long)val, 0, NTP1ADDRESS, NTP2ADDRESS, "");
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

    sntp_setservername(CUSTOM_NTP_INDEX, (char*)ntp);
    LOG(printf_P, PSTR("Set custom NTP to: %s\n"), ntp);
}

void TimeProcessor::attach_callback(callback_function_t callback){
    _timecallback = std::move(callback);
}
