# EmbUI
Embedded WebUI Interface

[![arduino-library-badge](https://www.ardu-badge.com/badge/EmbUI.svg?)](https://www.ardu-badge.com/EmbUI)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/dmytro%20korniienko/library/EmbUI.svg)](https://registry.platformio.org/libraries/dmytro%20korniienko/EmbUI)
[![PlatformIO CI](https://github.com/DmytroKorniienko/EmbUI/actions/workflows/pio_build_dev.yml/badge.svg)](https://github.com/DmytroKorniienko/EmbUI/actions/workflows/pio_build_dev.yml)
[![PlatformIO CI](https://github.com/DmytroKorniienko/EmbUI/actions/workflows/pio_build_main.yml/badge.svg)](https://github.com/DmytroKorniienko/EmbUI/actions/workflows/pio_build_main.yml)

__[CHANGELOG](/CHANGELOG.md)__

Framework for building a web-interface and controls for projects under Arduino ESP
## Supported controllers/platforms
 - ESP8266 Arduino Core
 - ESP32/ESP32-S2/ESP32-C3/ESP32-S3 Arduino Core

## Capabilities
 - automatic publication of the controller in the local network via [mDNS](https://en.wikipedia.org/wiki/Multicast_DNS)/[ZeroConf](https://en.wikipedia.org/wiki/Zero-configuration_networking)
 - device discovery capability:
    - [Service Browser](https://play.google.com/store/apps/details?id=com.druk.servicebrowser) Android
    - [SSDP](https://en.wikipedia.org/wiki/Simple_Service_Discovery_Protocol) for Windows
    - [Bonjour](https://en.wikipedia.org/wiki/Bonjour_(software)) iOS/MacOS
 - communication with the browser via WebSocket
 - support for multiple parallel connections, the interface is updated simultaneously on all devices
 - self-hosted - no dependencies on external resources/CDN/Cloud services
 - built-in WiFi manager, auto-switch to AP mode when client connection is lost
 - full support for all existing Time Zones, automatic changeover to summer/winter time, correct calculation of dates/time intervals
 - OTA, FS firmware/image update via browser
 - the ability to load data / interface elements via AJAX

## Projects based at EmbUI
 - [FireLamp_EmbUI](https://github.com/DmytroKorniienko/FireLamp_EmbUI/tree/dev) - ws2812 led matrix fire lamp
 - [ESPEM](https://github.com/vortigont/espem) - energy meter based on the PZEM-004 module


## Examples of building interfaces
![EmbUI](https://user-images.githubusercontent.com/26786760/140750180-65de2694-3ed8-41de-87eb-6f7e94ac12b2.png)
![image](https://user-images.githubusercontent.com/26786760/169590465-2947514b-a29f-446c-a8bb-16ab23abc912.png)

<details><summary>Other pictures</summary><img src="https://raw.githubusercontent.com/vortigont/espem/master/examples/espemembui.png" alt="espem ui" width="30%"/><img src="https://raw.githubusercontent.com/vortigont/espem/master/examples/espemembui_setup.png" alt="espem opts" width="30%"/></details>


## Usage
For WebUI to work, it is necessary to upload an image of the LittleFS file system with web resources to the controller.
Prepared resources for creating an image can be deployed [from the archive](https://github.com/DmytroKorniienko/EmbUI/raw/main/resources/data.zip).
In [Platformio](https://platformio.org/) this is usually the *data* directory at the root of the project.

## Depends

Projects           |                     URL                                                          | Remarks
------------------ | :-------------------------------------------------------------------------------:| --------------
ArduinoJson        |  https://github.com/bblanchon/ArduinoJson.git                                    |
AsyncWebServer-mod |  https://github.com/DmytroKorniienko/ESPAsyncWebServer/tree/ESPAsyncWebServerMod | manual install, fork
AsyncMqttClient    |  https://github.com/marvinroger/async-mqtt-client.git                            | manual install
TaskScheduler      |  https://github.com/arkhipenko/TaskScheduler.git                                 |
FtpClientServer    |  https://github.com/charno/FTPClientServer.git                                   | manual install, fork
ESP32SSDP          |  https://github.com/luc-github/ESP32SSDP.git                                     | manual install, esp32
AsyncTCP           |  https://github.com/me-no-dev/AsyncTCP.git                                       | manual install, esp32

