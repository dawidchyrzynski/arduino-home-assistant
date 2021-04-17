# Changelog

## 1.3.0

**New features:**
* Added `onMessage()` method to HAMqtt class
* Added support for HA Covers
* Added support for setting different prefix for non-discovery topics (see [Advanced MQTT example](examples/mqtt-advanced/mqtt-advanced.ino))
* Added `setName` method to HASensor
* Added `setName` method to HASwitch
* Added `onBeforeStateChanged` callback to HASwitch

**Updates:**
* Removed legacy properties from HAFan (Home Assistant 2021.4.4). Deprecated methods will be removed after a quarter (2021.7)
* Separated `uniqueID` field from `name` in all devices types

## 1.2.0

**Breaking changes:**
* Refactored HASensor implementation. Please take a look at [updated example](examples/sensor/sensor.ino)

**New features:**
* Added support for HVAC
* Added support for excluding devices types from the compilation using defines (see [src/ArduinoHADefines.h](src/ArduinoHADefines.h))
* Added support for setting icon in HASwitch and HASensor
* Added support for setting retain flag in HASwitch
* Added support for text (const char*) payload in HASensor
* Added support for fans (HAFan)
* Added support for connecting to the MQTT broker using hostname
* Added `onConnected()` method in the HAMqtt
* Added `onConnectionFailed()` method in the HAMqtt
* Added support for MQTT LWT (see [Advanced Availability example](examples/advanced-availability/advanced-availability.ino))

**Updates:**
* Optimized codebase and logic in all devices types
* Updated all examples
* Fixed compilation warnings in all classes
