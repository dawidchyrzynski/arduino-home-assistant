# Arduino Home Assistant integration

ArduinoHA allows to integrate an Arduino/ESP based device with Home Assistant using MQTT.
The library is designed to use as little resources (RAM/flash) as possible.
Initially it was optimized to work on Arduino Uno with Ethernet Shield,
but I successfully use it on ESP8266/ESP8255 boards in my projects.

## Features

* MQTT discovery (device is added to the Home Assistant panel automatically)
* MQTT Last Will and Testament
* Support for custom MQTT messages (publishing and subscribing)
* Auto reconnect with MQTT broker
* Reporting availability (online/offline states) of a device

## Supported HA types

* Binary sensors
* Covers
* Fans
* Device triggers
* Switches
* Sensors
* Tag scanner
* HVACs *(side note: HVACs requires more flash size than other HA types. It's not suitable for Arduino Nano/Uno)*

## Examples

* [Binary Sensor](examples/binary-sensor/binary-sensor.ino)
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

## Tested boards

* Arduino Uno
* Arduino Mega
* NodeMCU
* ESP-01
* Generic ESP8266/ESP8255
* Arduino Nano 33 IoT (SAMD)

## Tested devices

* Controllino Maxi (standard/pure/automation/power)
* Controllino Mega (standard/pure)
* Sonoff Dual R2
* Sonoff Basic
* Sonoff Mini
* Tuya Wi-Fi switch module
* Tuya Wi-Fi curtain module

## Tested Arduino Shields

* Arduino Ethernet Shield (WizNet W5100)

## Roadmap

* FAQ + Home Assistant setup instructions
* Documentation of the library
* Unit tests
* Reduce flash memory usage
* Add support for HA lights

## Unsupported features

The library doesn't support all features of the MQTT integration.
If you need support for a new feature please open a new issue in the repository.

# License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License v3.0 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/gpl.html
