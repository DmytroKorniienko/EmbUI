// localization resources goes here

#pragma once

/**
 *  Dictionary size
 *  must be more or equal to the number of messages in TD Enum
 */
#define DICT_SIZE 70

/**
 * Text-Dictionary Enums for language resources
 * the order of enums must match with elements in dictionary
 *
 */
enum TD : uint8_t {
    D_ACTION = (0U),
    D_ACTIVE,
    D_ADD,
    D_APOnlyMode,
    D_Begin,
    D_Conf,
    D_Confs,
    D_CONNECT,
    D_Create,
    D_DATETIME,
    D_DEBUG,
    D_EDIT,
    D_EVENT,
    D_EXIT,
    D_FWLOAD,
    D_HOLD,
    D_Hostname,
    D_LARROW,
    D_Load,
    D_More,
    D_MQTT,
    D_MQTT_Host,
    D_MQTT_Interval,
    D_MQTT_Port,
    D_MQTT_Topic,
    D_MSG_APOnly,
    D_MSG_APProtect,
    D_MSG_DATETIME,
    D_MSG_TZONE,
    D_MSG_TZSet01,
    D_NTP_Secondary,
    D_ONCE,
    D_ONOFF,
    D_OPT_TEXT,
    D_OTHER,
    D_Password,
    D_PIOOTA,
    D_PRESS,
    D_RARROW,
    D_REBOOT,
    D_REFRESH,
    D_REPEAT,
    D_SAVE,
    D_Send,
    D_SETTINGS,
    D_Text,
    D_Time,
    D_Update,
    D_UPLOAD,
    D_User,
    D_WiFi,
    D_WiFiAP,
    D_WiFiAPOpts,
    D_WiFiClient,
    D_WiFiClientOpts,
    D_WIFI_MQTT,
    D_WiFiSSID,
    D_W_SUN,
    D_W_MON,
    D_W_TUE,
    D_W_WED,
    D_W_THU,
    D_W_FRI,
    D_W_SAT,
    D_LANG
};



// English Strings (order does not matther)
static const char T_EN_LANG[] PROGMEM = "Interface language";
static const char T_EN_ACTION[] PROGMEM = "Action";
static const char T_EN_ACTIVE[] PROGMEM = "Active";
static const char T_EN_ADD[] PROGMEM = "Add";
static const char T_EN_APOnlyMode[] PROGMEM = "AP-only Mode";
static const char T_EN_Begin[] PROGMEM = "Begin";
static const char T_EN_Conf[] PROGMEM = "Configuration";
static const char T_EN_Confs[] PROGMEM = "Configurations";
static const char T_EN_CONNECT[] PROGMEM = "Connect";
static const char T_EN_Create[] PROGMEM = "Create";
static const char T_EN_DATETIME[] PROGMEM = "Date / Time / Time Zone";
static const char T_EN_DEBUG[] PROGMEM = "Debug";
static const char T_EN_EDIT[] PROGMEM = "Edit";
static const char T_EN_EVENT[] PROGMEM = "Event";
static const char T_EN_EXIT[] PROGMEM = "Exit";
static const char T_EN_FWLOAD[] PROGMEM = "Upload firmware/FS image";
static const char T_EN_HOLD[] PROGMEM = "Hold";
static const char T_EN_Hostname[] PROGMEM = "Hostname (mDNS Hostname/AP-SSID)";
static const char T_EN_LARROW[] PROGMEM = "<<<";
static const char T_EN_Load[] PROGMEM = "Load";
static const char T_EN_More[] PROGMEM = "More...";
static const char T_EN_MQTT_Host[] PROGMEM = "MQTT host";
static const char T_EN_MQTT_Interval[] PROGMEM = "MQTT update interval, sec.";
static const char T_EN_MQTT_Port[] PROGMEM = "MQTT port";
static const char T_EN_MQTT_Topic[] PROGMEM = "MQTT topic";
static const char T_EN_MQTT[] PROGMEM = "MQTT";
static const char T_EN_MSG_APOnly[] PROGMEM = "In AP-only EmbUI always works as an Access Point and never attempt any WiFi-client connections";
static const char T_EN_MSG_APProtect[] PROGMEM = "Protect AP with a password";
static const char T_EN_MSG_DATETIME[] PROGMEM = "Date/Time as YYYY-MM-DDThh:mm:ss (for autonomous devices)";
static const char T_EN_MSG_TZONE[] PROGMEM = "TZone rule (be sure to set this!)";
static const char T_EN_MSG_TZSet01[] PROGMEM = "TZSET rule for TimeZone/daylight setup (i.e. 'MSK-3' for Europe/Moscow) Rules could be found at https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h";
static const char T_EN_NTP_Secondary[] PROGMEM = "BackUp NTP-server (optional)";
static const char T_EN_ONCE[] PROGMEM = "Once";
static const char T_EN_ONOFF[] PROGMEM = "ON/OFF";
static const char T_EN_OPT_TEXT[] PROGMEM = "Argument (text)";
static const char T_EN_OTHER[] PROGMEM = "Other";
static const char T_EN_Password[] PROGMEM = "Password";
static const char T_EN_PIOOTA[] PROGMEM = "Update via ОТА-PIO";
static const char T_EN_PRESS[] PROGMEM = "KeyPress";
static const char T_EN_RARROW[] PROGMEM = ">>>";
static const char T_EN_REBOOT[] PROGMEM = "Reboot";
static const char T_EN_REFRESH[] PROGMEM = "Fefresh";
static const char T_EN_REPEAT[] PROGMEM = "Repeat";
static const char T_EN_SAVE[] PROGMEM = "Save";
static const char T_EN_Send[] PROGMEM = "Send";
static const char T_EN_SETTINGS[] PROGMEM = "Settings";
static const char T_EN_Text[] PROGMEM = "Text";
static const char T_EN_Time[] PROGMEM = "Time";
static const char T_EN_Update[] PROGMEM = "FW Update";
static const char T_EN_UPLOAD[] PROGMEM = "Upload";
static const char T_EN_User[] PROGMEM = "User";
static const char T_EN_WiFiAPOpts[] PROGMEM = "WiFi AP setup";
static const char T_EN_WiFiAP[] PROGMEM = "WiFi AP";
static const char T_EN_WiFiClientOpts[] PROGMEM = "WiFi-client setup";
static const char T_EN_WiFiClient[] PROGMEM = "WiFi Client";
static const char T_EN_WIFI_MQTT[] PROGMEM = "WiFi & MQTT";
static const char T_EN_WiFi[] PROGMEM = "WiFi";
static const char T_EN_WiFiSSID[] PROGMEM = "WiFi SSID";
static const char T_EN_W_SUN[] PROGMEM = "Sunday";
static const char T_EN_W_MON[] PROGMEM = "Monday";
static const char T_EN_W_TUE[] PROGMEM = "Tuesday";
static const char T_EN_W_WED[] PROGMEM = "Wednesday";
static const char T_EN_W_THU[] PROGMEM = "Thursday";
static const char T_EN_W_FRI[] PROGMEM = "Friday";
static const char T_EN_W_SAT[] PROGMEM = "Saturday";

// Русские тексты (порядок значения не имеет)
static const char T_RU_LANG[] PROGMEM = "Язык интерфейса";
static const char T_RU_ACTION[] PROGMEM = "Действие";
static const char T_RU_ACTIVE[] PROGMEM = "Активно";
static const char T_RU_ADD[] PROGMEM = "Добавить";
static const char T_RU_APOnlyMode[] PROGMEM = "Режим AP-only";
static const char T_RU_Begin[] PROGMEM = "Начать";
static const char T_RU_Conf[] PROGMEM = "Конфигурация";
static const char T_RU_Confs[] PROGMEM = "Конфигурации";
static const char T_RU_CONNECT[] PROGMEM = "Подключиться";
static const char T_RU_Create[] PROGMEM = "Создать";
static const char T_RU_DATETIME[] PROGMEM = "Date / Time / Time Zone";
static const char T_RU_DEBUG[] PROGMEM = "Отладка";
static const char T_RU_EDIT[] PROGMEM = "Редактировать";
static const char T_RU_EVENT[] PROGMEM = "Событие";
static const char T_RU_EXIT[] PROGMEM = "Выход";
static const char T_RU_FWLOAD[] PROGMEM = "Загрузка прошивки/образа FS";
static const char T_RU_HOLD[] PROGMEM = "Удержание";
static const char T_RU_Hostname[] PROGMEM = "Имя хоста (mDNS Hostname/AP-SSID)";
static const char T_RU_LOAD[] PROGMEM = "Загрузить";
static const char T_RU_More[] PROGMEM = "Еще...";
static const char T_RU_MQTT_Interval[] PROGMEM = "Интервал mqtt сек.";
static const char T_RU_MSG_APOnly[] PROGMEM = "В режиме AP-only лампа всегда работает как точка доступа и не будет подключаться к другим WiFi-сетям";
static const char T_RU_MSG_APProtect[] PROGMEM = "Защитить AP паролем";
static const char T_RU_MSG_DATETIME[] PROGMEM = "Дата/время в формате YYYY-MM-DDThh:mm:ss (если нет интернета)";
static const char T_RU_MSG_TZONE[] PROGMEM = "правило TZone (рекоммендуется задать!)";
static const char T_RU_MSG_TZSet01[] PROGMEM = "Правила TZSET учета поясного/сезонного времени (напр 'MSK-3' для Europe/Moscow) Нужную строку можно взять тут https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h";
static const char T_RU_NTP_Secondary[] PROGMEM = "резервный NTP-сервер (не обязательно)";
static const char T_RU_ONCE[] PROGMEM = "Однократно";
static const char T_RU_ONOFF[] PROGMEM = "Вкл/Выкл";
static const char T_RU_OPT_TEXT[] PROGMEM = "Параметр (текст)";
static const char T_RU_OTHER[] PROGMEM = "Другие";
static const char T_RU_Password[] PROGMEM = "Пароль";
static const char T_RU_PIOOTA[] PROGMEM = "Обновление по ОТА-PIO";
static const char T_RU_PRESS[] PROGMEM = "Нажатия";
static const char T_RU_REBOOT[] PROGMEM = "Перезагрузка";
static const char T_RU_REFRESH[] PROGMEM = "Обновить";
static const char T_RU_REPEAT[] PROGMEM = "Повтор";
static const char T_RU_SAVE[] PROGMEM = "Сохранить";
static const char T_RU_Send[] PROGMEM = "Отправить";
static const char T_RU_SETTINGS[] PROGMEM = "Настройки";
static const char T_RU_Text[] PROGMEM = "Текст";
static const char T_RU_Time[] PROGMEM = "Время";
static const char T_RU_Update[] PROGMEM = "Обновление ПО";
static const char T_RU_UPLOAD[] PROGMEM = "Загрузить";
static const char T_RU_User[] PROGMEM = "Пользователь";
static const char T_RU_WiFiAPOpts[] PROGMEM = "Настройки WiFi-точки доступа";
static const char T_RU_WiFiClientOpts[] PROGMEM = "Настройки WiFi-клиента";
static const char T_RU_WiFiClient[] PROGMEM = "WiFi-клиент";
static const char T_RU_W_MON[] PROGMEM = "Понедельник";
static const char T_RU_W_WED[] PROGMEM = "Среда";
static const char T_RU_W_TUE[] PROGMEM = "Вторник";
static const char T_RU_W_THU[] PROGMEM = "Четверг";
static const char T_RU_W_FRI[] PROGMEM = "Пятница";
static const char T_RU_W_SAT[] PROGMEM = "Суббота";
static const char T_RU_W_SUN[] PROGMEM = "Воскресенье";



/**
 *  Dictionary with references to all text resources
 *  it is a two-dim array of pointers to flash strings.
 *  Each row is a set of messages of a given language
 *  Each colums is a language index
 *  Messages indexes of each lang must match each other
 *  it is possible to reuse untraslated mesages from other lang's
 */
static const char *const T_DICT[][DICT_SIZE] PROGMEM = {
// Index 0 - Russian lang
  { T_RU_ACTION,
    T_RU_ACTIVE,
    T_RU_ADD,
    T_RU_APOnlyMode,
    T_RU_Begin,
    T_RU_Conf,
    T_RU_Confs,
    T_RU_CONNECT,
    T_RU_Create,
    T_RU_DATETIME,
    T_RU_DEBUG,
    T_RU_EDIT,
    T_RU_EVENT,
    T_RU_EXIT,
    T_RU_FWLOAD,
    T_RU_HOLD,
    T_RU_Hostname,
    T_EN_LARROW,
    T_RU_LOAD,
    T_RU_More,
    T_EN_MQTT,
    T_EN_MQTT_Host,
    T_RU_MQTT_Interval,
    T_EN_MQTT_Port,
    T_EN_MQTT_Topic,
    T_RU_MSG_APOnly,
    T_RU_MSG_APProtect,
    T_RU_MSG_DATETIME,
    T_RU_MSG_TZONE,
    T_RU_MSG_TZSet01,
    T_RU_NTP_Secondary,
    T_RU_ONCE,
    T_RU_ONOFF,
    T_RU_OPT_TEXT,
    T_RU_OTHER,
    T_RU_Password,
    T_RU_PIOOTA,
    T_RU_PRESS,
    T_EN_RARROW,
    T_RU_REBOOT,
    T_RU_REFRESH,
    T_RU_REPEAT,
    T_RU_SAVE,
    T_RU_Send,
    T_RU_SETTINGS,
    T_RU_Text,
    T_RU_Time,
    T_RU_Update,
    T_RU_UPLOAD,
    T_RU_User,
    T_EN_WiFi,
    T_EN_WiFiAP,
    T_RU_WiFiAPOpts,
    T_RU_WiFiClient,
    T_RU_WiFiClientOpts,
    T_EN_WIFI_MQTT,
    T_EN_WiFiSSID,
    T_RU_W_SUN,
    T_RU_W_MON,
    T_RU_W_TUE,
    T_RU_W_WED,
    T_RU_W_THU,
    T_RU_W_FRI,
    T_RU_W_SAT,
    T_RU_LANG},


// Index 1 - English lang
  { T_EN_ACTION,
    T_EN_ACTIVE,
    T_EN_ADD,
    T_EN_APOnlyMode,
    T_EN_Begin,
    T_EN_Conf,
    T_EN_Confs,
    T_EN_CONNECT,
    T_EN_Create,
    T_EN_DATETIME,
    T_EN_DEBUG,
    T_EN_EDIT,
    T_EN_EVENT,
    T_EN_EXIT,
    T_EN_FWLOAD,
    T_EN_HOLD,
    T_EN_Hostname,
    T_EN_LARROW,
    T_EN_Load,
    T_EN_More,
    T_EN_MQTT,
    T_EN_MQTT_Host,
    T_EN_MQTT_Interval,
    T_EN_MQTT_Port,
    T_EN_MQTT_Topic,
    T_EN_MSG_APOnly,
    T_EN_MSG_APProtect,
    T_EN_MSG_DATETIME,
    T_EN_MSG_TZONE,
    T_EN_MSG_TZSet01,
    T_EN_NTP_Secondary,
    T_EN_ONCE,
    T_EN_ONOFF,
    T_EN_OPT_TEXT,
    T_EN_OTHER,
    T_EN_Password,
    T_EN_PIOOTA,
    T_EN_PRESS,
    T_EN_RARROW,
    T_EN_REBOOT,
    T_EN_REFRESH,
    T_EN_REPEAT,
    T_EN_SAVE,
    T_EN_Send,
    T_EN_SETTINGS,
    T_EN_Text,
    T_EN_Time,
    T_EN_Update,
    T_EN_UPLOAD,
    T_EN_User,
    T_EN_WiFi,
    T_EN_WiFiAP,
    T_EN_WiFiAPOpts,
    T_EN_WiFiClient,
    T_EN_WiFiClientOpts,
    T_EN_WIFI_MQTT,
    T_EN_WiFiSSID,
    T_EN_W_SUN,
    T_EN_W_MON,
    T_EN_W_TUE,
    T_EN_W_WED,
    T_EN_W_THU,
    T_EN_W_FRI,
    T_EN_W_SAT,
    T_EN_LANG
  }
};