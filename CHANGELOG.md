# Changelog

## 2.0.0

**New features:**

* Added support for `icon` property in `HABinarySensor` (you can set icon using `HABinarySensor::setIcon("iconName")`)
* Added support for forcing `setState` in `HABinarySensor` using second argument as following `HABinarySensor::setState(true, true)`
* Added support for `device_class` property in `HACover` (you can set class using `HACover::setDeviceClass("className")`
* Added support for `icon` property in `HACover` (you can set icon using `HACover::setIcon("iconName")`)
* Added pointer of the sender to the `HACover` callback function
* Added support for forcing `setPosition` in `HACover` using second argument as following `HACover::setPosition(100, true)`
* Added support for `HAButton` device type
* Added support for `HADeviceTracker` device type
* Added support for `HACamera` device type
* Added support for `HALock` device type
* Added support for `device_class` property in the `HASwitch`
* Added support for `optimistic` property in the `HASwitch`
* Added support for `force_update` property in the `HASensor`

**Bugs fixes:**
* Last Will Message is now retained (#70)

**Breaking changes:**

* Changed structure of all MQTT topics used in the library.
* Changed constructor of the `HABinarySensor` class (removed `deviceClass` and `initialState` arguments)
* Replaced `HATriggers` with `HADeviceTrigger` - the new implementation is not backward compatible. Please check updated example of the `multi-state-button`.
* Renamed `HADevice::isOnline()` method to `HADevice::isAvailable()`
* Changed logic of the `HASwitch` callback. Please check the `led-switch` example.
* Refactored `HASensor` logic. It's now divided into three different classes: `HASensor`, `HASensorInteger` and `HASensorFloat`. This approach reduces flash size by ~2k
* Removed all legacy constructors with `HAMqtt` argument

## 1.3.0

**New features:**
* Added `onMessage()` method to HAMqtt class
* Added support for HA Covers
* Added support for setting different prefix for non-discovery topics (see [Advanced MQTT example](examples/mqtt-advanced/mqtt-advanced.ino))
* Added `setName` method to HASensor
* Added `setName` method to HASwitch
* Added `onBeforeStateChanged` callback to HASwitch

**Improvements:**
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

**Improvements:**
* Optimized codebase and logic in all devices types
* Updated all examples
* Fixed compilation warnings in all classes
