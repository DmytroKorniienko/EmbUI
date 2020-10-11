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
  embui.begin();
}

void loop() {
  embui.handle();
}