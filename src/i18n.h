// localization list = |en|ru|
// localization for BasicUI resources goes here

#ifndef _EMBUI_I18N_H
#define _EMBUI_I18N_H
#include <Arduino.h>
/**
 * the order of enums must match with elements in dictionary
 *
 */
// English Strings (order does not matter)

static const char TD_LANG[] PROGMEM =           "TD_001";   // en: "Interface language";  ru: "Язык интерфейса";
static const char TD_ACTION[] PROGMEM =         "TD_002";   // en: "Action";  ru: "Действие";
static const char TD_ACTIVE[] PROGMEM =         "TD_003";   // en: "Active";  ru: "Активно";
static const char TD_ADD[] PROGMEM =            "TD_004";   // en: "Add";  ru: "Добавить";
static const char TD_APPLY[] PROGMEM =          "TD_005";   // en: "Apply";  ru: "Применить";
static const char TD_WIFIMODE[] PROGMEM =       "TD_006";   // en: "WiFi Mode";  ru: "Режим WiFi";
static const char TD_BEGIN[] PROGMEM =          "TD_007";   // en: "Begin";  ru: "Начать";
static const char TD_CONF[] PROGMEM =           "TD_008";   // en: "Configuration";  ru: "Конфигурация";
static const char TD_CONFS[] PROGMEM =          "TD_009";   // en: "Configurations";  ru: "Конфигурации";
static const char TD_CONNECT[] PROGMEM =        "TD_010";   // en: "Connect";  ru: "Подключиться";
static const char TD_CREATE[] PROGMEM =         "TD_011";   // en: "Create";  ru: "Создать";
static const char TD_DATETIME[] PROGMEM =       "TD_012";   // en: "Date / Time / Time Zone";  ru: "Дата / Время / Часовая зона";
static const char TD_DEBUG[] PROGMEM =          "TD_013";   // en: "Debug";  ru: "Отладка";
static const char TD_EDIT[] PROGMEM =           "TD_014";   // en: "Edit";  ru: "Редактировать";
static const char TD_EVENT[] PROGMEM =          "TD_015";   // en: "Event";  ru: "Событие";
static const char TD_EXIT[] PROGMEM =           "TD_016";   // en: "Exit";  ru: "Выход";
static const char TD_FWLOAD[] PROGMEM =         "TD_017";   // en: "Upload firmware/FS image";  ru: "Загрузка прошивки/образа FS";
static const char TD_HOLD[] PROGMEM =           "TD_018";   // en: "Hold";  ru: "Удержание";
static const char TD_HOSTNAME[] PROGMEM =       "TD_019";   // en: "Hostname (mDNS Hostname/AP-SSID)";  ru: "Имя хоста (mDNS Hostname/AP-SSID)";
static const char TD_LARROW[] PROGMEM =         "TD_020";   // en: "<<<"; 
static const char TD_LOAD[] PROGMEM =           "TD_021";   // en: "Load";  ru: "Загрузить";
static const char TD_MORE[] PROGMEM =           "TD_022";   // en: "More...";  ru: "Еще...";
static const char TD_MQTT_HOST[] PROGMEM =      "TD_023";   // en: "MQTT host"; 
static const char TD_MQTT_INTERVAL[] PROGMEM =  "TD_024";   // en: "MQTT update interval, sec.";  ru: "Интервал mqtt сек.";
static const char TD_MQTT_PORT[] PROGMEM =      "TD_025";   // en: "MQTT port"; 
static const char TD_MQTT_PREFIX[] PROGMEM =    "TD_026";   // en: "MQTT prefix"; 
static const char TD_MQTT[] PROGMEM =           "TD_027";   // en: "MQTT"; 
static const char TD_MSG_WIFIMODE[] PROGMEM =   "TD_028";   // en: "In AP-only mode EmbUI always works as an Access Point and never attempt any WiFi-client connections, STA - client mode, STA+AP - mixed";  ru: "В режиме AP EmbUI всегда работает как точка доступа и не будет подключаться к другим WiFi-сетям, STA - режим клиента, AP+STA - смешанный";
static const char TD_MSG_APPROTECT[] PROGMEM =  "TD_029";   // en: "Protect AP with a password";  ru: "Защитить AP паролем";
static const char TD_MSG_DATETIME[] PROGMEM =   "TD_030";   // en: "Date/Time, YYYY-MM-DDThh:mm:ss (without internet connection, if empty - from device)";  ru: "Дата/время в формате YYYY-MM-DDThh:mm:ss (без интернета, если пусто - время с устройства)";
static const char TD_MSG_TZONE[] PROGMEM =      "TD_031";   // en: "Time zone";  ru: "Часовая зона";
static const char TD_MSG_TZSET01[] PROGMEM =    "TD_032";   // en: "TimeZone shift/daylight saving rules applied automatically, no need to adjust mannualy.";  ru: "Установки часовой зоны. Правила смены поясного/сезонного времени применяются автоматически, ручной коррекции не требуется. Если в вашей зоны нет в списке, можно выбрать общую зону сдвига от Гринвича";
static const char TD_NTP_SECONDARY[] PROGMEM =  "TD_033";   // en: "BackUp NTP-server (optional)";  ru: "резервный NTP-сервер (не обязательно)";
static const char TD_ONCE[] PROGMEM =           "TD_034";   // en: "Once";  ru: "Однократно";
static const char TD_ONOFF[] PROGMEM =          "TD_035";   // en: "ON/OFF";  ru: "Вкл/Выкл";
static const char TD_OPT_TEXT[] PROGMEM =       "TD_036";   // en: "Argument (text)";  ru: "Параметр (текст)";
static const char TD_OTHER[] PROGMEM =          "TD_037";   // en: "Other";  ru: "Другие";
static const char TD_PASSWORD[] PROGMEM =       "TD_038";   // en: "Password";  ru: "Пароль";
static const char TD_PIOOTA[] PROGMEM =         "TD_039";   // en: "Update via ОТА-PIO";  ru: "Обновление по ОТА-PIO";
static const char TD_PRESS[] PROGMEM =          "TD_040";   // en: "KeyPress";  ru: "Нажатия";
static const char TD_RARROW[] PROGMEM =         "TD_041";   // en: ">>>"; 
static const char TD_REBOOT[] PROGMEM =         "TD_042";   // en: "Reboot";  ru: "Перезагрузка";
static const char TD_REFRESH[] PROGMEM =        "TD_043";   // en: "Fefresh";  ru: "Обновить";
static const char TD_REPEAT[] PROGMEM =         "TD_044";   // en: "Repeat";  ru: "Повтор";
static const char TD_SAVE[] PROGMEM =           "TD_045";   // en: "Save";  ru: "Сохранить";
static const char TD_SEND[] PROGMEM =           "TD_046";   // en: "Send";  ru: "Отправить";
static const char TD_SETTINGS[] PROGMEM =       "TD_047";   // en: "Settings";  ru: "Настройки";
static const char TD_TEXT[] PROGMEM =           "TD_048";   // en: "Text";  ru: "Текст";
static const char TD_TIME[] PROGMEM =           "TD_049";   // en: "Time";  ru: "Время";
static const char TD_UPDATE[] PROGMEM =         "TD_050";   // en: "FW Update";  ru: "Обновление ПО";
static const char TD_UPLOAD[] PROGMEM =         "TD_051";   // en: "Upload";  ru: "Загрузить";
static const char TD_USER[] PROGMEM =           "TD_052";   // en: "User";  ru: "Пользователь";
static const char TD_WIFIAPOPTS[] PROGMEM =     "TD_053";   // en: "WiFi & AP setup";  ru: "Настройки WiFi и точки доступа";
static const char TD_WIFIAP[] PROGMEM =         "TD_054";   // en: "WiFi AP";  
static const char TD_WIFICLIENTOPTS[] PROGMEM = "TD_055";   // en: "WiFi-client setup";  ru: "Настройки WiFi-клиента";
static const char TD_WIFICLIENT[] PROGMEM =     "TD_056";   // en: "WiFi Client"; ru: "WiFi-клиент";

static const char TD_NOTSET057[] PROGMEM =      "TD_057";   // en: "not set";
static const char TD_NOTSET058[] PROGMEM =      "TD_058";   // en: "not set";

static const char TD_FTP[] PROGMEM =            "TD_059";   // en: "FTP";
static const char TD_WIFI[] PROGMEM =           "TD_060";   // en: "WiFi";
static const char TD_WIFISSID[] PROGMEM =       "TD_061";   // en: "WiFi SSID";
static const char TD_SCAN[] PROGMEM =           "TD_062";   // en: "Scan";  ru: "Сканировать";
static const char TD_W_SUN[] PROGMEM =          "TD_063";   // en: "Sunday";  ru: "Понедельник";
static const char TD_W_MON[] PROGMEM =          "TD_064";   // en: "Monday";  ru: "Вторник";
static const char TD_W_TUE[] PROGMEM =          "TD_065";   // en: "Tuesday";  ru: "Среда";
static const char TD_W_WED[] PROGMEM =          "TD_066";   // en: "Wednesday";  ru: "Четверг";;   
static const char TD_W_THU[] PROGMEM =          "TD_067";   // en: "Thursday";  ru: "Пятница";
static const char TD_W_FRI[] PROGMEM =          "TD_068";   // en: "Friday";  ru: "Суббота";
static const char TD_W_SAT[] PROGMEM =          "TD_069";   // en: "Saturday";  ru: "Воскресенье";
static const char TD_WIFI_STA[] PROGMEM =       "TD_070";   // en: "Client (STA)";  ru: "Клиент (STA)";
static const char TD_WIFI_AP[] PROGMEM =        "TD_071";   // en: "Access point (AP)";  ru: "Точка доступа (AP)";
static const char TD_WIFI_APSTA[] PROGMEM =     "TD_072";   // en: "Mixed (AP+STA)";  ru: "Смешанный (AP+STA)";
static const char TD_MSG_CONF[] PROGMEM =       "TD_073";   // en: "Sure?";  ru: "Уверены?";    

#if defined(EMBUI_USE_FTP) && defined(EMBUI_USE_MQTT)
static const char TD_WIFI_MQTT[] PROGMEM =      "TD_074";   // en: "WiFi, MQTT & FTP";
#elif defined(EMBUI_USE_MQTT)
static const char TD_WIFI_MQTT[] PROGMEM =      "TD_075";   // en: "WiFi & MQTT";
#elif defined(EMBUI_USE_FTP)
static const char TD_WIFI_MQTT[] PROGMEM =      "TD_076";   // en: "WiFi & FTP";
#else
static const char TD_WIFI_MQTT[] PROGMEM =      "TD_077";   // en: "WiFi";
#endif

#endif