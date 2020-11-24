/*
    Here is a set of predefined WebUI elements for system settings setup like WiFi, time, MQTT, etc...
*/
#pragma once

#include "ui.h"
#include "i18n.h"        // localized GUI text-strings

/**
 * List of UI languages in predefined i18n resources
 */
enum LANG : uint8_t {
    RU = (0U),
    EN = (1U),
};

static uint8_t lang = LANG::RU;   // default language for text resources

// UI blocks
static const char T_SETTINGS[] PROGMEM = "settings";
static const char T_OPT_NETW[] PROGMEM = "networking";

// UI handlers
static const char T_DO_OTAUPD[] PROGMEM = "update";
static const char T_SET_WIFI[] PROGMEM = "set_wifi";
static const char T_SET_WIFIAP[] PROGMEM = "set_wifiAP";
static const char T_SET_MQTT[] PROGMEM = "set_mqtt";
static const char T_SET_TIME[] PROGMEM = "set_time";

static const char T_SH_NETW[] PROGMEM = "sh_netw";
static const char T_SH_TIME[] PROGMEM = "sh_time";

/*
    A class with static functions to handle basic WebUI interface
*/
class BasicUI {

  public:

  /**
   * Define configuration variables and controls handlers
   * 
   * Variables has literal names and are kept within json-configuration file on flash
   * Control handlers are bound by literal name with a particular method. This method is invoked
   * by manipulating controls
   * 
   * this method owerrides weak definition in framework
   * 
   */
  static void add_sections();

  /**
   * This code adds "Settings" section to the MENU
   * it is up to you to properly open/close Interface json_section
   */
  static void opt_setup(Interface *interf, JsonObject *data);

    static void block_settings_netw(Interface *interf, JsonObject *data);
    static void block_settings_update(Interface *interf, JsonObject *data);
    static void block_settings_time(Interface *interf, JsonObject *data);

    static void section_settings_frame(Interface *interf, JsonObject *data);
    static void set_settings_wifi(Interface *interf, JsonObject *data);
    static void set_settings_wifiAP(Interface *interf, JsonObject *data);
    static void set_settings_mqtt(Interface *interf, JsonObject *data);
    static void set_settings_time(Interface *interf, JsonObject *data);
    static void set_language(Interface *interf, JsonObject *data);
    static void embuistatus(Interface *interf);

    //uint8_t uploadProgress(size_t len, size_t total);
};
