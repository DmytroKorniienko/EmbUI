/*
    Here is a set of predefined WebUI elements for system settings setup like WiFi, time, MQTT, etc...
*/
#pragma once

#include "ui.h"
#include "i18n.h"        // localized GUI text-strings
#include "constants.h" 

/**
 * List of UI languages in predefined i18n resources
 */
enum LANG : uint8_t {
    RU = (0U),
    EN = (1U),
};

/*
    перенакрываемая функция для добавления пользовательских пунктов в меню "Настройки"
    вызывается в конце section_settings_frame()
*/
void __attribute__((weak)) user_settings_frame(Interface *interf, JsonObject *data);
uint8_t __attribute__((weak)) uploadProgress(size_t len, size_t total);

/*
    A class with static functions to handle basic WebUI interface
*/
class BasicUI {
  private:
    static bool isBackOn;           // is returning to main settings? default=true
#ifndef ESP8266     
    static Task *_WIFIScan;
#endif
  public:
    static uint8_t lang;            // default language for text resources
  /**
   * Define configuration variables and controls handlers
   * 
   * Variables has literal names and are kept within json-configuration file on flash
   * Control handlers are bound by literal name with a particular method. This method is invoked
   * by manipulating controls
   * 
   */
  static void add_sections(bool skipBack=false);

  /**
   * This code adds "Settings" section to the MENU
   * it is up to you to properly open/close Interface json_section
   */
  static void opt_setup(Interface *interf, JsonObject *data);

    static void block_settings_netw(Interface *interf, JsonObject *data);
    static void block_only_wifi(Interface *interf, JsonObject *data);
    static void block_settings_update(Interface *interf, JsonObject *data);
    static void block_settings_time(Interface *interf, JsonObject *data);

    static void section_settings_frame(Interface *interf, JsonObject *data);
    static void set_settings_wifi(Interface *interf, JsonObject *data);
    static void set_scan_wifi(Interface *interf, JsonObject *data);
    static void set_settings_mqtt(Interface *interf, JsonObject *data);
    static void set_settings_time(Interface *interf, JsonObject *data);
    static void set_language(Interface *interf, JsonObject *data);
    static void embuistatus(Interface *interf);
    static void show_progress(Interface *interf, JsonObject *data);
    static void set_reboot(Interface *interf, JsonObject *data);
#ifdef EMBUI_USE_FTP
    static void set_ftp(Interface *interf, JsonObject *data);
    static void set_chk_ftp(Interface *interf, JsonObject *data);
#endif

    static void scan_complete(int n);

    //uint8_t uploadProgress(size_t len, size_t total);
};
