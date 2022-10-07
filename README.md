# Arduino Home Assistant integration 🏠

ArduinoHA allows to integrate an Arduino/ESP based device with Home Assistant using MQTT.
The library is designed to use as few resources (RAM/flash) as possible.
Initially, it was optimized to work on Arduino Uno with Ethernet Shield,
but I successfully use it on ESP8266/ESP8255 boards in my projects.

## Features

* Two-way communication (state reporting and command execution)
* MQTT discovery (device is added to the Home Assistant panel automatically)
* MQTT Last Will and Testament
* Support for custom MQTT messages (publishing and subscribing)
* Auto reconnect with MQTT broker
* Reporting availability (online/offline states) of a device
* Doxygen documentation for all classes
* Covered by unit tests (AUnit + EpoxyDuino + AUniter)

## Supported HA types

| Home Assistant type | Supported | Documentation |
| ------------------- | :--------: | :------: |
| Binary sensor       |     ✅     |     -    |
| Button              |     ✅     |     -    |
| Camera              |     ✅     |     -    |
| Cover               |     ✅     |     -    |
| Device tracker      |     ✅     |     -    |
| Device trigger      |     ✅     |     -    |
| Fan                 |     ✅     |     -    |
| Humidifier          |     ❌     |     -    |
| HVAC\*              |     ✅     |     -    |
| Light               |     ✅     |     -    |
| Lock                |     ✅     |     -    |
| Number              |     ✅     |     -    |
| Scene               |     ✅     |     -    |
| Select              |     ✅     |     -    |
| Sensor              |     ✅     |     -    |
| Switch              |     ✅     |     -    |
| Tag scanner         |     ✅     |     -    |
| Vacuum              |     ❌     |     -    |

\* - Requires more resources (flash / RAM) than other devices types.

## Examples

TO DO: table with all examples

## Compatible hardware

The library uses the Arduino Ethernet Client API for interacting with the network hardware.
It should work fine as long as the `Client` class is available.

Here is the list of devices on which the library was tested:

* Arduino Uno
* Arduino Mega
* Arduino Nano
* Arduino Pro Mini
* Arduino Nano 33 IoT
* NodeMCU
* Controllino Mega (Pure)
* Controllino Maxi (Pure)
* ESP-01
* ESP32-CAM
* Sonoff Dual R2
* Sonoff Dual R3
* Sonoff Basic
* Sonoff Mini
* Tuya Wi-Fi switch module
* Tuya Wi-Fi curtain module

Please note that it's not the complete list of supported devices.
You may try to use the library on any device that uses Arduino core.
