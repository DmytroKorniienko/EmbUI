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

// Include Tasker class from Framework to work with periodic tasks
#include "ts.h"

// TaskScheduler - Let the runner object be a global, single instance shared between object files.
extern Scheduler ts;

// Periodic task that runs every 5 sec and calls sensor publishing method
Task tDisplayUpdater(5 * TASK_SECOND, TASK_FOREVER, &sensorPublisher, &ts, true );

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

    /**
     * добавляем свои обрабочки на вывод UI-секций
     * и действий над данными
     */
    embui.section_handle_add(FPSTR(T_DEMO), block_demopage);                // generate "Demo" UI section

    // обработчики
    embui.section_handle_add(FPSTR(V_LED), action_blink);               // обработка рычажка светодиода
    embui.section_handle_add(FPSTR(V_UPDRATE), setRate);                 // sensor data publisher rate change

#if defined CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3
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
}

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
    interf->json_section_main(FPSTR(T_SET_DEMO), F("Some ESP32 demo sensors"));
#elif defined CONFIG_IDF_TARGET_ESP32S3
    LOG(println, F("CONFIG_IDF_TARGET_ESP32S3"));
    interf->json_section_main(FPSTR(T_DEMO), F("Some ESP32-S3 demo controls"));
#elif defined CONFIG_IDF_TARGET_ESP32S2  
    LOG(println, F("CONFIG_IDF_TARGET_ESP32S2"));
    interf->json_section_main(FPSTR(T_SET_DEMO), F("Some ESP32-S2 demo sensors"));
#elif defined CONFIG_IDF_TARGET_ESP32C3  
    LOG(println, F("CONFIG_IDF_TARGET_ESP32C3"));
    interf->json_section_main(FPSTR(T_SET_DEMO), F("Some ESP32-C3 demo sensors"));
#else
    LOG(println, F("ESP8266"));
    interf->json_section_main(FPSTR(T_SET_DEMO), F("Some demo sensors"));
#endif  

    // переключатель, связанный со светодиодом. Изменяется синхронно
    interf->checkbox(FPSTR(V_LED), F("Onboard LED"), true);

    interf->comment(F("Комментарий: набор сенсоров для демонстрации"));     // комментарий-описание секции

    interf->json_section_line();             // "Live displays"

    // Voltage display, shows ESPs internal voltage
#ifdef ESP8266
    interf->display(F("vcc"), String(ESP.getVcc()/1000.0));
#else
    interf->display(F("vcc"), String("3.3"));
    //interf->display(F("vcc"), String((float)rom_phy_get_vdd33()/1000.0)); // extern "C" int rom_phy_get_vdd33();
#endif
    // Fake temperature sensor
    interf->display(F("temp"), String(24));
    interf->json_section_end();     // end of line

    // Simple Clock display
    String clk; embui.timeProcessor.getDateTimeString(clk);
    interf->display(F("clk"), clk);    // Clock sensor

    /*
      Some display based on user defined CSS class - "mycssclass". CSS must be loaded in WebUI
      Resulting CSS classes are defined as: class='mycssclass vcc'
      So both could be used to customize display appearance 
    interf->display(F("vcc"), 42, "mycssclass");
    */

    // Update rate slider
    interf->range(FPSTR(V_UPDRATE), String(tDisplayUpdater.getInterval()/1000), String(1), String(30), String(1), String(F("Update Rate, sec")), true);

    interf->json_section_end();
    interf->json_frame_flush();
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

/**
 * Call-back for Periodic publisher 
 * it reads (virtual) sensors and publishes values to the WebUI
 */
void sensorPublisher() {
    if (!embui.ws.count())
      return;

    Interface *interf = new Interface(EmbUI::GetInstance(), &EmbUI::GetInstance()->ws, EMBUI_SMALL_JSON_SIZE);
    interf->json_frame_value();
    // Voltage sensor
    //  id, value, html=true

#if defined CONFIG_IDF_TARGET_ESP32  
    interf->value(F("vcc"), String((float)rom_phy_get_vdd33()/1000.0)); // extern "C" int rom_phy_get_vdd33();
    interf->value(F("temp"), String(24 + random(-30,30)/10), true);                // add some random spikes to the temperature :)
#elif defined CONFIG_IDF_TARGET_ESP32S2  
    interf->value(F("vcc"), String("3.3"), true); // html must be set 'true' so this value could be handeled properly for div elements
    float t;
    if(temp_sensor_read_celsius(&t)==ESP_OK){
      interf->value(F("temp"), String(t,1), true);
    }
#elif defined CONFIG_IDF_TARGET_ESP32C3  
    interf->value(F("vcc"), String("3.3"), true); // html must be set 'true' so this value could be handeled properly for div elements
    float t;
    if(temp_sensor_read_celsius(&t)==ESP_OK){
      interf->value(F("temp"), String(t,1), true);
    }
#else
    interf->value(F("vcc"), String((ESP.getVcc() + random(-100,100))/1000.0), true); // html must be set 'true' so this value could be handeled properly for div elements
    interf->value(F("temp"), String(24 + random(-30,30)/10), true);                // add some random spikes to the temperature :)
#endif  

    String clk; embui.timeProcessor.getDateTimeString(clk);
    interf->value(F("clk"), clk, true); // Current date/time for Clock display

    interf->json_frame_flush();
    delete interf;
}

/**
 * Change sensor update rate callback
 */
void setRate(Interface *interf, JsonObject *data) {
  if (!data) return;

  tDisplayUpdater.setInterval( (*data)[FPSTR(V_UPDRATE)].as<unsigned int>() * 1000 ); // set update rate in seconds
}