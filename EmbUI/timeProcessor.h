// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#pragma once
#define __TIMEPROCESSOR_H

#include "globals.h"

#ifndef TZONE
    #include <Ticker.h>
#endif

#include "wi-fi.h"

#ifdef CONTRY
    #define NTP2ADDRESS        COUNTRY "." "pool.ntp.org"    // пул серверов времени для NTP
    #define NTP1ADDRESS        "ntp3.vniiftri.ru"
#else
    #define NTP1ADDRESS        "ntp3.vniiftri.ru"
    #define NTP2ADDRESS        ("pool.ntp.org")
#endif

#define CUSTOM_NTP_INDEX    2
#define TIMEAPI_BUFSIZE     600
#define TM_BASE_YEAR        1900
#define DAYSECONDS          (86400U)
#define DATETIME_STRLEN     (19U)   // buffer for data/time string "YYYY-MM-DDThh:mm:ss"
#define HTTPSYNC_DELAY      5
#define HTTP_REFRESH_HRS    3     // время суток для обновления часовой зоны
#define HTTP_REFRESH_MIN    3

// http://worldtimeapi.org/api/ip
// http://worldtimeapi.org/api/timezone
static const char PG_timeapi_tz_url[] PROGMEM  = "http://worldtimeapi.org/api/timezone/";
static const char PG_timeapi_ip_url[] PROGMEM  = "http://worldtimeapi.org/api/ip";

class TimeProcessor
{
private:
    TimeProcessor(const TimeProcessor&);            // noncopyable
    TimeProcessor& operator=(const TimeProcessor&); // noncopyable

#ifndef TZONE
    Ticker _wrk;        // планировщик
    /**
     * Функция обращается к внешнему http-сервису, получает временную зону/летнее время
     * на основании либо установленной переменной tzone, либо на основе IP-адреса
     * в случае если временная зона содержит правила перехода на летнее время, функция
     * запускает планировщик для автокоррекции временной зоны ежесуточно в 3 часа ночи
     */
    void getTimeHTTP();
    unsigned int getHttpData(String &payload, const String &url);
    /**
     * функция установки планировщика обновления временной зоны
     * при вызове без параметра выставляет отложенный запуск на HTTP_REFRESH_HRS:HTTP_REFRESH_MIN
     */
    void httprefreshtimer(const uint32_t delay=0);
#endif

protected:
    callback_function_t _timecallback = nullptr;
    //WiFiEventHandler eGotIPHandler, eDisconnectHandler;  // ручки для отслеживания состояния WiFi

    String tzone;            // строка зоны для http-сервиса как она задана в https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.csv

    bool isSynced = false;      // флаг, означает что время было синхронизированно

    // используем http-сервис для смещени TZ
    #ifdef TZONE
        bool usehttpzone = false;
    #else
        bool usehttpzone = true;
    #endif

    virtual void timeavailable();       // колбэк установки времени

public:
    TimeProcessor();

    /**
     * обратный вызов при подключении к WiFi точке доступа
     * запускает синхронизацию времени
     */
#ifdef ESP8266
    void onSTAGotIP(WiFiEventStationModeGotIP ipInfo);
    void onSTADisconnected(WiFiEventStationModeDisconnected event_info);
#endif

#ifdef ESP32
    void WiFiEvent(WiFiEvent_t event, system_event_info_t info);
#endif


    /**
     * установка строки с текущей временной зоной в текстовом виде,
     * влияет, на запрос через http-api за временем в конкретной зоне,
     * вместо автоопределения по ip
     * !ВНИМАНИЕ! Никакого отношения к текущей системной часовой зоне эта функция не имеет!!! 
     */
    void httpTimezone(const char *var);

    /**
     * Функция установки системного времени, принимает в качестве аргумента указатель на строку в формате
     * "YYYY-MM-DDThh:mm:ss"
     */
    static void setTime(const char *timestr);
    static void setTime(const String &timestr);

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
    void tzsetup(const char* tz);

    /**
     * установка пользовательского ntp-сервера
     * сервер имеет низший приоритет
     * @param ntp - сервер в виде ip или hostname
     */
    void setcustomntp(const char* ntp);

    /**
     *  установка смещения текущего системного времени от UTC в секундах
     *  можно применять если не получается выставить нормально зону
     *  и правила перехода сезонного времени каким-либо другим способом.
     *  При уставке правила перехода сезонного времени (если были) сбрасываются!
     */
    void setOffset(const int val);

    void attach_callback(callback_function_t callback);

    void dettach_callback(){
        _timecallback = nullptr;
    }

    /**
     * Возвращает текущее смещение локального системного времени от UTC в секундах
     * с учетом часовой зоны и правил смены сезонного времени (если эти параметры были
     * корректно установленны ранее каким-либо методом)
     */
    static long int getOffset();

    /**
     *  возвращает текуший unixtime
     */
    static time_t getUnixTime() {return time(nullptr); }

    /**
     * возвращает строку с временем в формате "00:00"
     */
    String getFormattedShortTime();

    int getHours()
    {
        return localtime(now())->tm_hour;
    }

    int getMinutes()
    {
        return localtime(now())->tm_min;
    }

    /**
     * возвращает true если врямя еще не было синхронизированно каким либо из доступных источников
     */
    bool isDirtyTime() {return !isSynced;}

    /**
     * функция допечатывает в переданную строку заданный таймстамп в дату/время в формате "9999-99-99T99:99"
     * @param _tstamp - преобразовать заданный таймстамп, если не задан используется текущее локальное время
     */
    static void getDateTimeString(String &buf, const time_t _tstamp = 0);

    /**
     * returns pointer to current unixtime
     * (удобно использовать для передачи в localtime())
     */
    static const time_t* now(){
        static time_t _now;
        time(&_now);
        return &_now;
    }

    /**
     * возвращает true если текущее время :00 секунд 
     */
    static bool seconds00(){
        if ((localtime(now())->tm_sec))
          return false;
        else
          return true;
    }

};
