// localization for BasicUI resources goes here

#pragma once

/**
 * Text-Dictionary Enums for language resources
 * the order of enums must match with elements in dictionary
 *
 */
enum TD : uint8_t {
    D_ACTION = (0U),
    D_ACTIVE,
    D_ADD,
    D_APPLY,
    D_WIFIMODE,
    D_BEGIN,
    D_CONF,
    D_CONFS,
    D_CONNECT,
    D_CREATE,
    D_DATETIME,
    D_DEBUG,
    D_EDIT,
    D_EVENT,
    D_EXIT,
    D_FWLOAD,
    D_HOLD,
    D_HOSTNAME,
    D_LARROW,
    D_LOAD,
    D_MORE,
    D_MQTT,
    D_MQTT_HOST,
    D_MQTT_INTERVAL,
    D_MQTT_PORT,
    D_MQTT_PREFIX,
    D_MSG_WIFIMODE,
    D_MSG_APPROTECT,
    D_MSG_DATETIME,
    D_MSG_TZONE,
    D_MSG_TZSET01,
    D_NTP_SECONDARY,
    D_ONCE,
    D_ONOFF,
    D_OPT_TEXT,
    D_OTHER,
    D_PASSWORD,
    D_PIOOTA,
    D_PRESS,
    D_RARROW,
    D_REBOOT,
    D_REFRESH,
    D_REPEAT,
    D_SAVE,
    D_SEND,
    D_SETTINGS,
    D_TEXT,
    D_TIME,
    D_UPDATE,
    D_UPLOAD,
    D_USER,
    D_WIFI,
    D_WIFIAP,
    D_WIFIAPOPTS,
    D_WIFICLIENT,
    D_WIFICLIENTOPTS,
    D_WIFI_MQTT,
    D_WIFISSID,
    D_W_SUN,
    D_W_MON,
    D_W_TUE,
    D_W_WED,
    D_W_THU,
    D_W_FRI,
    D_W_SAT,
    D_LANG,
    D_SCAN,
    D_FTP,
    D_WIFI_STA,
    D_WIFI_AP,
    D_WIFI_APSTA,
    D_MSG_CONF,
    DICT_SIZE               // Должен быть в конце списка!
};



// English Strings (order does not matter)
static const char T_EN_LANG[] PROGMEM = "Interface language";
static const char T_EN_ACTION[] PROGMEM = "Action";
static const char T_EN_ACTIVE[] PROGMEM = "Active";
static const char T_EN_ADD[] PROGMEM = "Add";
static const char T_EN_APPLY[] PROGMEM = "Apply";
static const char T_EN_WIFIMODE[] PROGMEM = "WiFi Mode";
static const char T_EN_BEGIN[] PROGMEM = "Begin";
static const char T_EN_CONF[] PROGMEM = "Configuration";
static const char T_EN_CONFS[] PROGMEM = "Configurations";
static const char T_EN_CONNECT[] PROGMEM = "Connect";
static const char T_EN_CREATE[] PROGMEM = "Create";
static const char T_EN_DATETIME[] PROGMEM = "Date / Time / Time Zone";
static const char T_EN_DEBUG[] PROGMEM = "Debug";
static const char T_EN_EDIT[] PROGMEM = "Edit";
static const char T_EN_EVENT[] PROGMEM = "Event";
static const char T_EN_EXIT[] PROGMEM = "Exit";
static const char T_EN_FWLOAD[] PROGMEM = "Upload firmware/FS image";
static const char T_EN_HOLD[] PROGMEM = "Hold";
static const char T_EN_HOSTNAME[] PROGMEM = "Hostname (mDNS Hostname/AP-SSID)";
static const char T_EN_LARROW[] PROGMEM = "<<<";
static const char T_EN_LOAD[] PROGMEM = "Load";
static const char T_EN_MORE[] PROGMEM = "More...";
static const char T_EN_MQTT_HOST[] PROGMEM = "MQTT host";
static const char T_EN_MQTT_INTERVAL[] PROGMEM = "MQTT update interval, sec.";
static const char T_EN_MQTT_PORT[] PROGMEM = "MQTT port";
static const char T_EN_MQTT_PREFIX[] PROGMEM = "MQTT prefix";
static const char T_EN_MQTT[] PROGMEM = "MQTT";
static const char T_EN_MSG_WIFIMODE[] PROGMEM = "In AP-only EmbUI always works as an Access Point and never attempt any WiFi-client connections, STA - client mode, STA+AP - mixed";
static const char T_EN_MSG_APPROTECT[] PROGMEM = "Protect AP with a password";
static const char T_EN_MSG_DATETIME[] PROGMEM = "Date/Time, YYYY-MM-DDThh:mm:ss (without internet connection, if empty - from device)";
static const char T_EN_MSG_TZONE[] PROGMEM = "Time zone";
static const char T_EN_MSG_TZSET01[] PROGMEM = "TimeZone shift/daylight saving rules applied automatically, no need to adjust mannualy.";
static const char T_EN_NTP_SECONDARY[] PROGMEM = "BackUp NTP-server (optional)";
static const char T_EN_ONCE[] PROGMEM = "Once";
static const char T_EN_ONOFF[] PROGMEM = "ON/OFF";
static const char T_EN_OPT_TEXT[] PROGMEM = "Argument (text)";
static const char T_EN_OTHER[] PROGMEM = "Other";
static const char T_EN_PASSWORD[] PROGMEM = "Password";
static const char T_EN_PIOOTA[] PROGMEM = "Update via ОТА-PIO";
static const char T_EN_PRESS[] PROGMEM = "KeyPress";
static const char T_EN_RARROW[] PROGMEM = ">>>";
static const char T_EN_REBOOT[] PROGMEM = "Reboot";
static const char T_EN_REFRESH[] PROGMEM = "Fefresh";
static const char T_EN_REPEAT[] PROGMEM = "Repeat";
static const char T_EN_SAVE[] PROGMEM = "Save";
static const char T_EN_SEND[] PROGMEM = "Send";
static const char T_EN_SETTINGS[] PROGMEM = "Settings";
static const char T_EN_TEXT[] PROGMEM = "Text";
static const char T_EN_TIME[] PROGMEM = "Time";
static const char T_EN_UPDATE[] PROGMEM = "FW Update";
static const char T_EN_UPLOAD[] PROGMEM = "Upload";
static const char T_EN_USER[] PROGMEM = "User";
static const char T_EN_WIFIAPOPTS[] PROGMEM = "WiFi & AP setup";
static const char T_EN_WIFIAP[] PROGMEM = "WiFi AP";
static const char T_EN_WIFICLIENTOPTS[] PROGMEM = "WiFi-client setup";
static const char T_EN_WIFICLIENT[] PROGMEM = "WiFi Client";
#ifdef EMBUI_USE_FTP
static const char T_EN_WIFI_MQTT[] PROGMEM = "WiFi, MQTT & FTP";
#else
static const char T_EN_WIFI_MQTT[] PROGMEM = "WiFi & MQTT";
#endif
static const char T_EN_FTP[] PROGMEM = "FTP";
static const char T_EN_WIFI[] PROGMEM = "WiFi";
static const char T_EN_WIFISSID[] PROGMEM = "WiFi SSID";
static const char T_EN_SCAN[] PROGMEM = "Scan";
static const char T_EN_W_SUN[] PROGMEM = "Sunday";
static const char T_EN_W_MON[] PROGMEM = "Monday";
static const char T_EN_W_TUE[] PROGMEM = "Tuesday";
static const char T_EN_W_WED[] PROGMEM = "Wednesday";
static const char T_EN_W_THU[] PROGMEM = "Thursday";
static const char T_EN_W_FRI[] PROGMEM = "Friday";
static const char T_EN_W_SAT[] PROGMEM = "Saturday";
static const char T_EN_WIFI_STA[] PROGMEM = "Client (STA)";
static const char T_EN_WIFI_AP[] PROGMEM = "Access point (AP)";
static const char T_EN_WIFI_APSTA[] PROGMEM = "Mixed (AP+STA)";
static const char T_EN_MSG_CONF[] PROGMEM = "Sure?";

// Русские тексты (порядок значения не имеет)
static const char T_RU_LANG[] PROGMEM = "Язык интерфейса";
static const char T_RU_ACTION[] PROGMEM = "Действие";
static const char T_RU_ACTIVE[] PROGMEM = "Активно";
static const char T_RU_ADD[] PROGMEM = "Добавить";
static const char T_RU_APPLY[] PROGMEM = "Применить";
static const char T_RU_WIFIMODE[] PROGMEM = "Режим WiFi";
static const char T_RU_BEGIN[] PROGMEM = "Начать";
static const char T_RU_CONF[] PROGMEM = "Конфигурация";
static const char T_RU_CONFS[] PROGMEM = "Конфигурации";
static const char T_RU_CONNECT[] PROGMEM = "Подключиться";
static const char T_RU_CREATE[] PROGMEM = "Создать";
static const char T_RU_DATETIME[] PROGMEM = "Дата / Время / Часовая зона";
static const char T_RU_DEBUG[] PROGMEM = "Отладка";
static const char T_RU_EDIT[] PROGMEM = "Редактировать";
static const char T_RU_EVENT[] PROGMEM = "Событие";
static const char T_RU_EXIT[] PROGMEM = "Выход";
static const char T_RU_FWLOAD[] PROGMEM = "Загрузка прошивки/образа FS";
static const char T_RU_HOLD[] PROGMEM = "Удержание";
static const char T_RU_HOSTNAME[] PROGMEM = "Имя хоста (mDNS Hostname/AP-SSID)";
static const char T_RU_LOAD[] PROGMEM = "Загрузить";
static const char T_RU_MORE[] PROGMEM = "Еще...";
static const char T_RU_MQTT_INTERVAL[] PROGMEM = "Интервал mqtt сек.";
static const char T_RU_MSG_WIFIMODE[] PROGMEM = "В режиме AP лампа всегда работает как точка доступа и не будет подключаться к другим WiFi-сетям, STA - режим клиента, AP+STA - смешанный";
static const char T_RU_MSG_APPROTECT[] PROGMEM = "Защитить AP паролем";
static const char T_RU_MSG_DATETIME[] PROGMEM = "Дата/время в формате YYYY-MM-DDThh:mm:ss (без интернета, если пусто - время с устройства)";
static const char T_RU_MSG_TZONE[] PROGMEM = "Часовая зона";
static const char T_RU_MSG_TZSET01[] PROGMEM = "Установки часовой зоны. Правила смены поясного/сезонного времени применяются автоматически, ручной коррекции не требуется. Если в вашей зоны нет в списке, можно выбрать общую зону сдвига от Гринвича";
static const char T_RU_NTP_SECONDARY[] PROGMEM = "резервный NTP-сервер (не обязательно)";
static const char T_RU_ONCE[] PROGMEM = "Однократно";
static const char T_RU_ONOFF[] PROGMEM = "Вкл/Выкл";
static const char T_RU_OPT_TEXT[] PROGMEM = "Параметр (текст)";
static const char T_RU_OTHER[] PROGMEM = "Другие";
static const char T_RU_PASSWORD[] PROGMEM = "Пароль";
static const char T_RU_PIOOTA[] PROGMEM = "Обновление по ОТА-PIO";
static const char T_RU_PRESS[] PROGMEM = "Нажатия";
static const char T_RU_REBOOT[] PROGMEM = "Перезагрузка";
static const char T_RU_REFRESH[] PROGMEM = "Обновить";
static const char T_RU_REPEAT[] PROGMEM = "Повтор";
static const char T_RU_SAVE[] PROGMEM = "Сохранить";
static const char T_RU_SEND[] PROGMEM = "Отправить";
static const char T_RU_SETTINGS[] PROGMEM = "Настройки";
static const char T_RU_TEXT[] PROGMEM = "Текст";
static const char T_RU_TIME[] PROGMEM = "Время";
static const char T_RU_UPDATE[] PROGMEM = "Обновление ПО";
static const char T_RU_UPLOAD[] PROGMEM = "Загрузить";
static const char T_RU_USER[] PROGMEM = "Пользователь";
static const char T_RU_WIFIAPOPTS[] PROGMEM = "Настройки WiFi и точки доступа";
static const char T_RU_WIFICLIENTOPTS[] PROGMEM = "Настройки WiFi-клиента";
static const char T_RU_WIFICLIENT[] PROGMEM = "WiFi-клиент";
static const char T_RU_SCAN[] PROGMEM = "Сканировать";
static const char T_RU_W_MON[] PROGMEM = "Понедельник";
static const char T_RU_W_TUE[] PROGMEM = "Вторник";
static const char T_RU_W_WED[] PROGMEM = "Среда";
static const char T_RU_W_THU[] PROGMEM = "Четверг";
static const char T_RU_W_FRI[] PROGMEM = "Пятница";
static const char T_RU_W_SAT[] PROGMEM = "Суббота";
static const char T_RU_W_SUN[] PROGMEM = "Воскресенье";
static const char T_RU_WIFI_STA[] PROGMEM = "Клиент (STA)";
static const char T_RU_WIFI_AP[] PROGMEM = "Точка доступа (AP)";
static const char T_RU_WIFI_APSTA[] PROGMEM = "Смешанный (AP+STA)";
static const char T_RU_MSG_CONF[] PROGMEM = "Уверены?";

/**
 *  Dictionary with references to all text resources
 *  it is a two-dim array of pointers to flash strings.
 *  Each row is a set of messages of a given language
 *  Each colums is a language index
 *  Message indexes of each lang must match each otherб
 *  it is possible to reuse untraslated mesages from other lang's
 */
static const char *const T_DICT[][DICT_SIZE] PROGMEM = {
// Index 0 - Russian lang
  { T_RU_ACTION,
    T_RU_ACTIVE,
    T_RU_ADD,
    T_RU_APPLY,
    T_RU_WIFIMODE,
    T_RU_BEGIN,
    T_RU_CONF,
    T_RU_CONFS,
    T_RU_CONNECT,
    T_RU_CREATE,
    T_RU_DATETIME,
    T_RU_DEBUG,
    T_RU_EDIT,
    T_RU_EVENT,
    T_RU_EXIT,
    T_RU_FWLOAD,
    T_RU_HOLD,
    T_RU_HOSTNAME,
    T_EN_LARROW,
    T_RU_LOAD,
    T_RU_MORE,
    T_EN_MQTT,
    T_EN_MQTT_HOST,
    T_RU_MQTT_INTERVAL,
    T_EN_MQTT_PORT,
    T_EN_MQTT_PREFIX,
    T_RU_MSG_WIFIMODE,
    T_RU_MSG_APPROTECT,
    T_RU_MSG_DATETIME,
    T_RU_MSG_TZONE,
    T_RU_MSG_TZSET01,
    T_RU_NTP_SECONDARY,
    T_RU_ONCE,
    T_RU_ONOFF,
    T_RU_OPT_TEXT,
    T_RU_OTHER,
    T_RU_PASSWORD,
    T_RU_PIOOTA,
    T_RU_PRESS,
    T_EN_RARROW,
    T_RU_REBOOT,
    T_RU_REFRESH,
    T_RU_REPEAT,
    T_RU_SAVE,
    T_RU_SEND,
    T_RU_SETTINGS,
    T_RU_TEXT,
    T_RU_TIME,
    T_RU_UPDATE,
    T_RU_UPLOAD,
    T_RU_USER,
    T_EN_WIFI,
    T_EN_WIFIAP,
    T_RU_WIFIAPOPTS,
    T_RU_WIFICLIENT,
    T_RU_WIFICLIENTOPTS,
    T_EN_WIFI_MQTT,
    T_EN_WIFISSID,
    T_RU_W_SUN,
    T_RU_W_MON,
    T_RU_W_TUE,
    T_RU_W_WED,
    T_RU_W_THU,
    T_RU_W_FRI,
    T_RU_W_SAT,
    T_RU_LANG,
    T_RU_SCAN,
    T_EN_FTP,
    T_RU_WIFI_STA,
    T_RU_WIFI_AP,
    T_RU_WIFI_APSTA,
    T_RU_MSG_CONF
    },


// Index 1 - English lang
  { T_EN_ACTION,
    T_EN_ACTIVE,
    T_EN_ADD,
    T_EN_APPLY,
    T_EN_WIFIMODE,
    T_EN_BEGIN,
    T_EN_CONF,
    T_EN_CONFS,
    T_EN_CONNECT,
    T_EN_CREATE,
    T_EN_DATETIME,
    T_EN_DEBUG,
    T_EN_EDIT,
    T_EN_EVENT,
    T_EN_EXIT,
    T_EN_FWLOAD,
    T_EN_HOLD,
    T_EN_HOSTNAME,
    T_EN_LARROW,
    T_EN_LOAD,
    T_EN_MORE,
    T_EN_MQTT,
    T_EN_MQTT_HOST,
    T_EN_MQTT_INTERVAL,
    T_EN_MQTT_PORT,
    T_EN_MQTT_PREFIX,
    T_EN_MSG_WIFIMODE,
    T_EN_MSG_APPROTECT,
    T_EN_MSG_DATETIME,
    T_EN_MSG_TZONE,
    T_EN_MSG_TZSET01,
    T_EN_NTP_SECONDARY,
    T_EN_ONCE,
    T_EN_ONOFF,
    T_EN_OPT_TEXT,
    T_EN_OTHER,
    T_EN_PASSWORD,
    T_EN_PIOOTA,
    T_EN_PRESS,
    T_EN_RARROW,
    T_EN_REBOOT,
    T_EN_REFRESH,
    T_EN_REPEAT,
    T_EN_SAVE,
    T_EN_SEND,
    T_EN_SETTINGS,
    T_EN_TEXT,
    T_EN_TIME,
    T_EN_UPDATE,
    T_EN_UPLOAD,
    T_EN_USER,
    T_EN_WIFI,
    T_EN_WIFIAP,
    T_EN_WIFIAPOPTS,
    T_EN_WIFICLIENT,
    T_EN_WIFICLIENTOPTS,
    T_EN_WIFI_MQTT,
    T_EN_WIFISSID,
    T_EN_W_SUN,
    T_EN_W_MON,
    T_EN_W_TUE,
    T_EN_W_WED,
    T_EN_W_THU,
    T_EN_W_FRI,
    T_EN_W_SAT,
    T_EN_LANG,
    T_EN_SCAN,
    T_EN_FTP,
    T_EN_WIFI_STA,
    T_EN_WIFI_AP,
    T_EN_WIFI_APSTA,
    T_EN_MSG_CONF
  }
};