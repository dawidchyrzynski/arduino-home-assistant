# Arduino Home Assistant

ArduinoHA allows to integrate an Arduino/ESP based device with Home Assistant using MQTT.
The library is designed to use as little resources (RAM/flash) as possible.
Initially it was optimized to work on Arduino Uno with Ethernet Shield,
but I successfully use it on ESP8266/ESP8255 boards in my projects.

## Examples

* [Binary Sensor](examples/binary-sensor/binary-sensor.ino)
* [LED switch](examples/led-switch/led-switch.ino)
* [MQTT with credentials](examples/mqtt-with-credentials/mqtt-with-credentials.ino)
* [Multi-state button](examples/multi-state-button/multi-state-button.ino)
* [RFID tag reader](examples/rfid-tag-reader/rfid-tag-reader.ino)
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
