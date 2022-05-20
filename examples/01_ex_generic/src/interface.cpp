#include "main.h"

#include "EmbUI.h"
#include "interface.h"

#include "uistrings.h"   // non-localized text-strings

#ifdef ESP32
#if defined CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
  #include <driver/temp_sensor.h>
#endif
  extern "C" int rom_phy_get_vdd33();
#else
  ADC_MODE(ADC_VCC);  // read internal Vcc
#endif

/**
 * можно нарисовать свой собственный интефейс/обработчики с нуля, либо
 * подключить статический класс с готовыми формами для базовых системных натсроек и дополнить интерфейс.
 * необходимо помнить что существуют системные переменные конфигурации с зарезервированными именами.
 * Список имен системных переменных можно найти в файле "constants.h"
 */
#include "basicui.h"
/**
 * переопределяем метод из фреймворка, регистрирующий необходимы нам в проекте переменные и методы обработки
 * 
 */
void create_parameters(){
    LOG(println, F("UI: Creating application vars"));

   /**
    * регистрируем статические секции для web-интерфейса с системными настройками,
    * сюда входит:
    *  - WiFi-manager
    *  - установка часового пояса/правил перехода сезонного времени
    *  - установка текущей даты/времени вручную
    *  - базовые настройки MQTT
    *  - OTA обновление прошивки и образа файловой системы
    */
    BasicUI::add_sections();

    /**
     * регистрируем свои переменные
     */
    embui.var_create(FPSTR(V_LED), "1");    // LED default status is on
    embui.var_create(FPSTR(V_VAR1), "");    // заводим пустую переменную по умолчанию
    embui.var_create(FPSTR(V_VAR3), "0");

    /**
     * добавляем свои обрабочки на вывод UI-секций
     * и действий над данными
     */
    embui.section_handle_add(FPSTR(T_DEMO), block_demopage);                // generate "Demo" UI section

    // обработчики
    embui.section_handle_add(FPSTR(T_SET_DEMO), action_demopage);           // обработка данных из секции "Demo"

    embui.section_handle_add(FPSTR(V_LED), action_blink);               // обработка рычажка светодиода

#if defined CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3 // || CONFIG_IDF_TARGET_ESP32S3
    // ESP32-C3 & ESP32-S2
    {
      temp_sensor_config_t cfg = TSENS_CONFIG_DEFAULT();
      temp_sensor_set_config(cfg);
      temp_sensor_start();
    }
#endif
};

/**
 * Headlile section
 * this is an overriden weak method that builds our WebUI interface from the top
 * ==
 * Головная секция,
 * переопределенный метод фреймфорка, который начинает строить корень нашего Web-интерфейса
 * 
 */
void section_main_frame(Interface *interf, JsonObject *data){
  if (!interf) return;

  interf->json_frame_interface(FPSTR(T_HEADLINE));  // HEADLINE for EmbUI project page

  block_menu(interf, data);                         // Строим UI блок с меню выбора других секций
  interf->json_frame_flush();

  if(!embui.sysData.wifi_sta && embui.param(FPSTR(P_WIFIMODE))!="1"){
    // форсируем выбор вкладки настройки WiFi если контроллер не подключен к внешней AP
    LOG(println, F("UI: Opening network setup section"));
    BasicUI::block_settings_netw(interf, data);
  } else {
    block_demopage(interf, data);                   // Строим блок с demo переключателями
  }

  //block_more(interf, data);                     // у нас есть и другие блоки, но строить сразу все

};


/**
 * This code builds UI section with menu block on the left
 * 
 */
void block_menu(Interface *interf, JsonObject *data){
    if (!interf) return;
    // создаем меню
    interf->json_section_menu();

    /**
     * пункт меню - "демо"
     */
    interf->option(FPSTR(T_DEMO), F("UI Demo"));

    /**
     * добавляем в меню пункт - настройки,
     * это автоматически даст доступ ко всем связанным секциям с интерфейсом для системных настроек
     * 
     */
    BasicUI::opt_setup(interf, data);       // пункт меню "настройки"
    interf->json_section_end();
}

/**
 * Demo controls
 * 
 */
void block_demopage(Interface *interf, JsonObject *data){
    if (!interf) return;
    interf->json_frame_interface();

    // Headline
    // параметр FPSTR(T_SET_DEMO) определяет зарегистрированный обработчик данных для секции
#if defined CONFIG_IDF_TARGET_ESP32  
    LOG(println, F("CONFIG_IDF_TARGET_ESP32"));  
    interf->json_section_main(FPSTR(T_DEMO), F("Some ESP32 demo controls"));
#elif defined CONFIG_IDF_TARGET_ESP32S3
    LOG(println, F("CONFIG_IDF_TARGET_ESP32S3"));
    interf->json_section_main(FPSTR(T_DEMO), F("Some ESP32-S3 demo controls"));
#elif defined CONFIG_IDF_TARGET_ESP32S2  
    LOG(println, F("CONFIG_IDF_TARGET_ESP32S2"));
    interf->json_section_main(FPSTR(T_DEMO), F("Some ESP32-S2 demo controls"));
#elif defined CONFIG_IDF_TARGET_ESP32C3  
    LOG(println, F("CONFIG_IDF_TARGET_ESP32C3"));
    interf->json_section_main(FPSTR(T_DEMO), F("Some ESP32-C3 demo controls"));
#else
    LOG(println, F("ESP8266"));
    interf->json_section_main(FPSTR(T_SET_DEMO), F("Some demo controls"));
#endif
      interf->json_section_begin("", ""); // отдельная секция для светодиода, чтобы не мешало обработчику секции T_SET_DEMO для полей ниже
        interf->comment(F("Комментарий: набор контролов для демонстрации"));     // комментарий-описание секции

        // переключатель, связанный со светодиодом. Изменяется асинхронно 
        interf->checkbox(FPSTR(V_LED), F("Onboard LED"), true);
      interf->json_section_end();
      interf->json_section_begin(FPSTR(T_SET_DEMO), "");
        interf->text(FPSTR(V_VAR1), F("Текстовое поле"));                                 // текстовое поле со значением переменной из конфигурации
        interf->text(FPSTR(V_VAR2), String(F("some default val")), F("Второе текстовое поле"), false);   // текстовое поле со значением "по-умолчанию"
        interf->checkbox(FPSTR(V_VAR3), F("Зависимый переключатель, введите on или off во второе поле ввода"));
        /*  кнопка отправки данных секции на обработку
        *  первый параметр FPSTR(T_DEMO) определяет какая секция откроется
        *  после обработки отправленных данных
        */ 
        interf->button_submit(FPSTR(T_SET_DEMO), FPSTR(TD_SEND), FPSTR(P_GRAY));
      interf->json_section_end();
    interf->json_section_end();
    interf->json_frame_flush();
}

// сеттер для веб-контрола
void set_checkbox3(Interface *interf, JsonObject *data){
  Serial.printf_P(PSTR("Varialble_3 checkbox state after var2 check:%s\n"), (*data)[FPSTR(V_VAR3)]=="1"?PSTR("true"):PSTR("false"));
  SETPARAM(FPSTR(V_VAR3)); // записать значение в конфиг
}

void action_demopage(Interface *interf, JsonObject *data){
    if (!data) return;

    LOG(println, F("porcessig section demo"));

    // сохраняем значение 1-й переменной в конфиг фреймворка
    SETPARAM(FPSTR(V_VAR1));

    // выводим значение 1-й переменной в serial
    const char *text = (*data)[FPSTR(V_VAR1)];
    Serial.printf_P(PSTR("Varialble_1 value:%s\n"), text );

    // берем указатель на 2-ю переменную
    text = (*data)[FPSTR(V_VAR2)];
    // или так:
    // String var2 = (*data)[FPSTR(V_VAR2)];
    // выводим значение 2-й переменной в serial
    Serial.printf_P(PSTR("Varialble_2 value:%s\n"), text);

    Serial.printf_P(PSTR("Varialble_3 checkbox state after send:%s\n"), (*data)[FPSTR(V_VAR3)]=="1"?PSTR("true"):PSTR("false"));

    // для примера реализуем здесь зависимое поведение, если в строке записано "on" - включим чекбокс, если "off" - выключим, иначе ничего не делаем
    DynamicJsonDocument doc(512);
    JsonObject obj = doc.to<JsonObject>();
    if(String(text)=="on"){
      CALL_INTF(FPSTR(V_VAR3),"1",set_checkbox3);
    } else if(String(text)=="off"){
      CALL_INTF(FPSTR(V_VAR3),"0",set_checkbox3);
    }
}

void action_blink(Interface *interf, JsonObject *data){
  if (!data) return;  // здесь обрабатывает только данные

  SETPARAM(FPSTR(V_LED));  // save new LED state to the config

  // set LED state to the new checkbox state
  digitalWrite(LED_BUILTIN, !(*data)[FPSTR(V_LED)].as<unsigned int>()); // write inversed signal for builtin LED
  Serial.printf("LED: %d\n", (*data)[FPSTR(V_LED)].as<unsigned int>());
}

/**
 * обработчик статуса (периодического опроса контроллера веб-приложением)
 */
void pubCallback(Interface *interf){
    BasicUI::embuistatus(interf);
}
