# Unit tests

## Requirements

* [Arduino CLI](https://www.arduino.cc/pro/cli)
* [AUnit library](https://github.com/bxparks/AUnit)
* [AUniter tools](https://github.com/bxparks/AUniter)
* [EpoxyDuino](https://github.com/bxparks/EpoxyDuino)
* [ESP8266 core](https://arduino-esp8266.readthedocs.io/en/latest/installing.html)
* [ESP32 core](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html#installing-using-arduino-ide)
* [Arduino SAMD Core 1.8.9](https://github.com/arduino/ArduinoCore-samd)

## Running tests using EpoxyDuino

1. Open Terminal
2. Go to the `tests` directory
3. Run `make clean && make tests && make runtests`
4. Wait for the results

## Running tests on physical devices

1. Open Terminal
2. Go to the `tests` directory
3. Run `auniter test [ENV]:[USB port] *Test`, for example: `auniter test esp8266:.usbserial-1110 *Test`
4. Wait for the results
