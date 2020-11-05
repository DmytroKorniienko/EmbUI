#pragma once


/**
 * List of languages available
 */
enum LANG : uint8_t {
    RU = (0U),
    EN = (1U),
};

void block_menu(Interface *interf, JsonObject *data);
void block_settings_netw(Interface *interf, JsonObject *data);
void block_settings_update(Interface *interf, JsonObject *data);
void block_settings_time(Interface *interf, JsonObject *data);

void section_settings_frame(Interface *interf, JsonObject *data);
void set_settings_wifi(Interface *interf, JsonObject *data);
void set_settings_wifiAP(Interface *interf, JsonObject *data);
void set_settings_mqtt(Interface *interf, JsonObject *data);
void set_settings_time(Interface *interf, JsonObject *data);
void set_language(Interface *interf, JsonObject *data);


//void save_lamp_flags();

//void remote_action(RA action, ...);

//uint8_t uploadProgress(size_t len, size_t total);
