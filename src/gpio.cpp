// This framework originaly based on JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov
// then re-written and named by (c) 2020 Anton Zolotarev (obliterator) (https://github.com/anton-zolotarev)
// also many thanks to Vortigont (https://github.com/vortigont), kDn (https://github.com/DmytroKorniienko)
// and others people

#include "EmbUI.h"

/*
под esp32 статусы WiFi извлекаются по-другому,
если нужно будет, позже вернусь. пока убираю

void EmbUI::led_handle(){
    if (sysData.LED_PIN == 31) return;
    if (wifi_mode == WIFI_STA)
        digitalWrite(sysData.LED_PIN, !digitalRead(sysData.LED_PIN));
    if (wifi_mode == WIFI_AP)
        digitalWrite(sysData.LED_PIN, LOW + sysData.LED_INVERT);
}
*/

void EmbUI::btn(){
#ifdef __BUTTON
    if (digitalRead(__BUTTON))
        return;

    //uint8_t old_wifi_mode = wifi_mode;

    led_inv();
    unsigned long t = millis();
    bool i = false;
    while (!digitalRead(__BUTTON))
    {
        delay(1);
        if (t + 5000 < millis()) // Нажатие 5 секунд
        {
            if(!i){
                led_inv();
                i = true;
            }
            /*
            для чего тут меняется режим WiFi в обход методов из wi-fi.cpp не ясно
            if (old_wifi_mode == WIFI_STA)
                wifi_mode = WIFI_AP;
            if (old_wifi_mode == WIFI_AP)
                wifi_mode = WIFI_STA;
            */
        }
        if (t + 15000 < millis()) // Нажатие 10 секунд
        {
            led_inv();
            LittleFS.remove(F("/config.json"));
            ESP.restart();
        }
    }
/*
для чего тут меняется режим WiFi в обход методов из wi-fi.cpp не ясно

    if (wifi_mode != old_wifi_mode) // событие после отпуска кнопки от 5 секунд
    {
        if (wifi_mode == WIFI_STA)
            var(FPSTR(P_wifi), F("AP"));
        if (wifi_mode == WIFI_AP)
            var(FPSTR(P_wifi), F("STA"));
            // тут сохранить настройки
        wifi_connect();
        //ESP.restart();
    }
*/
#endif
}

void EmbUI::led_on(){
    if (sysData.LED_PIN == 31) return;
    digitalWrite(sysData.LED_PIN, false);
}

void EmbUI::led_off(){
    if (sysData.LED_PIN == 31) return;
    digitalWrite(sysData.LED_PIN, true);
}

void EmbUI::led_inv(){
    if (sysData.LED_PIN == 31) return;
    digitalWrite(sysData.LED_PIN, !digitalRead(sysData.LED_PIN));
}
