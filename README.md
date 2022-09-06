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
* Covered by unit tests (~100% coverage)

## Supported HA types

| Home Assistant type | Supported | Documentation |
| ------------------- | :-------: | :------: |
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

\* - Requires more resources (flash / RAM) than other devices types.

## Examples

* [Binary Sensor](examples/binary-sensor/binary-sensor.ino)
* [Button](examples/button/button.ino)
* [Cover](examples/cover/cover.ino)
* [Fan](examples/fan/fan.ino)
* [LED switch](examples/led-switch/led-switch.ino)
* [Multi-state button](examples/multi-state-button/multi-state-button.ino)
* [Sensor (temperature, humidity, etc.)](examples/sensor/sensor.ino)
* [HVAC](examples/hvac/hvac.ino)
* [NodeMCU Wi-Fi](examples/nodemcu/nodemcu.ino)
* [Arduino Nano 33 IoT Wi-Fi (SAMD)](examples/nano33iot/nano33iot.ino)
* [Availability feature](examples/availability)
* [Advanced availability (MQTT LWT)](examples/advanced-availability/advanced-availability.ino)
* [MQTT with credentials](examples/mqtt-with-credentials/mqtt-with-credentials.ino)
* [MQTT advanced](examples/mqtt-advanced/mqtt-advanced.ino)

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

# License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License v3.0 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/gpl.html
