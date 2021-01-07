# Arduino Home Assistant

ArduinoHA allows to integrate an Arduino/ESP based device with Home Assistant using MQTT.
The library is designed to use as little resources (RAM/flash) as possible.
Initially it was optimized to work on Arduino Uno with Ethernet Shield,
but I successfully use it on ESP8266/ESP8255 boards in my projects.

## Features

* MQTT discovery (device is added to the Home Assistant panel automatically)
* Auto reconnect with MQTT broker

## Examples

* [Binary Sensor](examples/binary-sensor/binary-sensor.ino)
* [LED switch](examples/led-switch/led-switch.ino)
* [MQTT with credentials](examples/mqtt-with-credentials/mqtt-with-credentials.ino)
* [Multi-state button](examples/multi-state-button/multi-state-button.ino)
* [Sensor (temperature, humidity, etc.)](examples/sensor/sensor.ino)
* [NodeMCU Wi-Fi](examples/nodemcu/nodemcu.ino)

## Tested boards

* Arduino Uno
* Arduino Mega
* Controllino Maxi (standard/pure/automation/power)
* Controllino Mega (standard/pure)
* NodeMCU
* ESP-01
* Generic ESP8266/ESP8255

## Tested Arduino Shields

* Arduino Ethernet Shield (WizNet W5100)

## Supported HA types

* Binary sensors
* Device triggers
* Switches
* Sensors
* Tag scanner

## Unsupported features

The library doesn't support all features of the MQTT integration.
If you need support for a new feature please open a new issue in the repository.

# License

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License v3.0 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/gpl.html
