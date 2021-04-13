// Все эти дефайны возможно нужно будет выносить в platformio.ini для корректной работы
// #define EMBUI_DEBUG                                               // Режим отладки
// #define DELAY_AFTER_FS_WRITING       (50U)                        // 50мс, меньшие значения могут повлиять на стабильность
// #define __DISABLE_BUTTON0                                         // Отключение кнопки "FLASH" на NODE_MCU
// #define __IDPREFIX F("EmbUI-")                                    // Префикс фреймворка
// #define __CFGSIZE (2048)                                          // Размер динамической памяти под глобальную конфигруацию
// #define EXTERNAL_SSDP
// #define __SSDPNAME ("EmbUI (kDn)")
// #define __SSDPURLMODEL ("https://github.com/DmytroKorniienko/")
// #define __SSDPMODEL ("https://github.com/DmytroKorniienko/")
// #define __SSDPURLMANUF ("https://github.com/anton-zolotarev")
// #define __SSDPMANUF ("obliterator")

#include <Arduino.h>
#include "EmbUI.h"

int val = 5;

void set_chk(Interface *interf, JsonObject *data){
    if(!data) return;
    SETPARAM("chk"); // отображен на конфиг
}

void set_by_btn(Interface *interf, JsonObject *data){
    if(!data) return;
    val = (*data)[F("rng")].as<int>();
}

void create_parameters(){
    LOG(println, F("Создание дефолтных параметров"));
    // создаем дефолтные параметры для нашего проекта, то что создано через var_create сохраняется в конфиг
    embui.var_create(F("chk"), "false");

    // обработчики
    embui.section_handle_add(F("chk"), set_chk);
    embui.section_handle_add(F("btn"), set_by_btn);
}

void block_effects_main(Interface *interf, JsonObject *data, bool fast=true){
    if (!interf) return;
    interf->json_section_main(F("Tab1"), F("Демонстрация"));

    interf->json_section_line(F("Контролы"));
    interf->checkbox(F("chk"), F("Переключатель"), true);
    interf->button(F("btn"), F("Кнопка"));
    interf->json_section_end();
    interf->range(F("rng"), val, 1, 10, 2, F("Ползунок"), true);

    interf->json_section_end();
}

void block_menu(Interface *interf, JsonObject *data){
    if (!interf) return;
    // создаем меню
    interf->json_section_menu();
    interf->option(F("Tab1"), F("Вкладка"));
    interf->json_section_end();
}

void section_main_frame(Interface *interf, JsonObject *data){
    if (!interf) return;

    interf->json_frame_interface(F("EmbUI Demo"));

    block_menu(interf, data);
    block_effects_main(interf, data);

    interf->json_frame_flush();
}

void pubCallback(Interface *interf){
    if (!interf) return;
    //return; // Временно для увеличения стабильности. Пока разбираюсь с падениями.
    interf->json_frame_value();
    interf->value(F("pTime"), F("00:00:01"), true);
    interf->value(F("pMem"), String(ESP.getFreeHeap()), true);
    interf->value(F("pUptime"), String(millis()/1000), true);
    interf->json_frame_flush();
}

String httpCallback(const String &param, const String &value, bool isset){
    LOG(printf_P, PSTR("HTTP: %s - %s\n"), param.c_str(), value.c_str());
    embui.publish(String(F("embui/pub/")) + param,value,false); // отправляем обратно в MQTT в топик embui/pub/
    return String();
}

void setup() {
  Serial.begin(115200);

  embui.udp(); // Ответ на UDP запрс. в качестве аргумента - переменная, содержащая macid (по умолчанию)

#if defined(ESP8266) && defined(LED_BUILTIN_AUX) && !defined(__DISABLE_BUTTON0)
  embui.led(LED_BUILTIN_AUX, false); // назначаем пин на светодиод, который нам будет говорит о состоянии устройства. (быстро мигает - пытается подключиться к точке доступа, просто горит (или не горит) - подключен к точке доступа, мигает нормально - запущена своя точка доступа)
#elif defined(__DISABLE_BUTTON0)
  embui.led(LED_BUILTIN, false); // Если матрица находится на этом же пине, то будет ее моргание!
#endif

  embui.init();
  //embui.mqtt(jee.param(F("m_host")), jee.param(F("m_port")).toInt(), jee.param(F("m_user")), jee.param(F("m_pass")), mqttCallback, true); // false - никакой автоподписки!!!
  
  create_parameters(); // создать параметры, после init(), перед begin()
  embui.begin();
}

void loop() {
  embui.handle();
}