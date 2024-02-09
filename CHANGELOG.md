# Changelog

## 2.1.0

**New features:**
* Added support for setting MQTT's keep alive [#153](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/153) by [@allenb2800](https://github.com/allenb2800)
* Added support for the `state_class` property in the `HASensor` [#179](https://github.com/dawidchyrzynski/arduino-home-assistant/pull/179)
* Implemented extended unique ID support for all device types. This allows you to prefix each device type's unique ID with the device ID, ensuring smooth deployment of identical code on multiple devices without encountering unique ID conflicts [#212](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/212)
* Added support for the `configuration_url` property in the `HADevice` [#182](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/182)
* Added `getCurrentOption` method to the `HASelect` device type [#163](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/163)
* Added support for publishing the `None` state in the `HASelect` device type [#146](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/146)
* Added support for publishing the `None` state in the `HASensor` device type [#175](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/175)
* Added support for the `expire_after` property in the `HASensor` [#171](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/171)
* Added support for the `expire_after` property in the `HABinarySensor` [#159](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/159)
* Added support for the JSON attributes in the `HASensor`
* Added support for the `object_id` property in all device types
* Added `setBufferSize` method to the `HAMqtt` class [#202](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/202)
* Added `getState` method to the `HAMqtt` class
* Added `onDisconnected` callback method to the `HAMqtt` class
* Added `onStateChanged` callback method to the `HAMqtt` class

**Fixes:**
* Fixed a bug with the maximum number of device types [#190](https://github.com/dawidchyrzynski/arduino-home-assistant/issues/190) by [@martaisty](https://github.com/martaisty)
* Fixed compiler warning: `class 'HANumeric' is implicitly friends with itself` [#197](https://github.com/dawidchyrzynski/arduino-home-assistant/pull/197) by [@shedokan](https://github.com/shedokan)
* The default limit for device types has been raised to 24 on processors other than ATMega328/ATMega168. The previous default limit of 6 led to confusion for many users.

## 2.0.0

**New features:**

* Added support for the `icon` property in the `HABinarySensor` (you can set the icon using `HABinarySensor::setIcon("iconName")`)
* Added support for changing the current state of the `HABinarySensor` using `HABinarySensor::setCurrentState` method
* Added support for forcing `setState` in `HABinarySensor` using a second argument as follows `HABinarySensor::setState(true, true)`
* Added support for the `device_class` property in the `HACover` (you can set the class using `HACover::setDeviceClass("className")`
* Added support for the `icon` property in the `HACover` (you can set the icon using `HACover::setIcon("iconName")`)
* Added pointer of the sender to the `HACover` callback function
* Added support for `optimistic` property in the `HACover` (you can change the mode using `HACover::setOptimistic(true)`)
* Added support for forcing `setPosition` in `HACover` using a second argument as follows `HACover::setPosition(100, true)`
* Added support for the `device_class` property in the `HASwitch` (you can set the class using `HASwitch::setDeviceClass("className")`
* Added support for the `optimistic` property in the `HASwitch` (you can change the mode using `HASwitch::setOptimistic(true)`)
* Added support for the `force_update` property in the `HASensor` (you can set the mode using `HASensor::setForceUpdate(true)`)
* Added support for the `HAButton` device type
* Added support for the `HADeviceTracker` device type
* Added support for the `HACamera` device type
* Added support for the `HALock` device type
* Added support for the `HASelect` device type
* Added support for the `HANumber` device type
* Added support for the `HAScene` device type
* Added support for the `HALight` device type

**Fixes:**
* Last Will Message is now retained (#70)
* Compilation error on SAMD family (#82)

**New examples:**
* [Button](examples/button/button.ino) - adding simple buttons to the Home Assistant panel.

**Breaking changes:**

* Changed structure of all MQTT topics used in the library.
* Changed constructor of the `HABinarySensor` class (removed `deviceClass` and `initialState` arguments)
* Renamed `HABinarySensor::getState()` method to `HABinarySensor::getCurrentState()`
* Replaced `HATriggers` with `HADeviceTrigger` - the new implementation is not backward compatible. Please check the updated example of the `multi-state-button`.
* Renamed `HADevice::isOnline()` method to `HADevice::isAvailable()`
* Renamed `HASwitch::onStateChanged` method to `HASwitch::onCommand`.
* Renamed `HAFan::onStateChanged` method to `HAFan::onStateCommand`.
* Renamed `HAFan::onSpeedChanged` method to `HAFan::onSpeedCommand`.
* Changed logic of the `HASwitch` callback. Please check the `led-switch` example.
* Refactored `HASensor` logic. It's now divided into two different classes: `HASensor` and `HASensorNumber`.
* Removed all legacy constructors with `HAMqtt` argument
* Removed `onConnectionFailed` callback from the `HAMqtt` class
* The position in the `HACover` is now available as configurable feature. It's disabled by default.
* Refactored `HAHVAC` class to support more features of the MQTT discovery. Please check the update example.

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
