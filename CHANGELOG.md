## Changelog

### v2.7.0  Release
Added:


### v2.6.0  Release
Added:
  - Multi languages support
  - Resources fix
  - Refactoring & optimization

### v2.5.0  Release
Added:
  - ESP32-C3 & ESP32-S2 support

Fixes:
  - stabilty and bug fixes

### v2.4.7  Release
Added:
  - Image Control

Fixes:
  - SDK3.0.0 WiFi fix
  - WiFi list fix
  - Examples fix

### v2.4.5  Release
Added:
  - RTC time for AP mode
  - Custom CSS display mode (Vortigont)

Fixes:
  - NTP ring for (no features mode)
  - cpp templates for html elements removed due issues
  - etc...

### v2.4.0  Release
Added:
  - Optimizations:
    - frameSend Class now can serialize json directly to the ws buffer
    - ws post data processing
    - no data echo on single ws client
    - config file save/load optimization
  - method allows actions unregistering
  - ws packet 'xload' triggers ajax request for block: object
  - Ticker has been replaced with TaskScheduler lib
  - cpp templates for html elements
  - custom named section mapped to <div> template
  - websocket rawdata packet type
  - Time Zones list loading via ajax

Fixes:
  - ws post dangling pointer fix
  - mqtt
  - range int/float overloads
  - WiFi ap/sta switching
  - sending current date/time from browser

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
