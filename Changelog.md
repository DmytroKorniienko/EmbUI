## Changelog

### v2.2.0  Release

##### WEB/HTML:
- dark/light themes

##### ESP32:
- OTA for ESP32 fixed for fw/fsimages. LittleFS requires 3-rd party tool to generate image
- Upload progress fix
- DHCP-client workaround for hostname missing in requests (espressif/arduino-esp32#2537)
 - fixed updating WiFi station variable (ESP32)
 
##### ESP32/esp8266:
- Delayed reconnection, allows websocket to finish section transaction. Fix #1
- TimeLib: Workaround for newlib bug with <+-nn> timezone names (esp8266/Arduino#7702)

##### EMBUI:
- Embed system-dependent  settings vars/GUI into framework code
  - predefined code for 'settings' web-page
  - generic example changed to adopt predefined settings UI

##### + other fixes:
- mqtt default pub time
- etc...
