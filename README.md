# Arduino Home Assistant integration 🏠

[![](https://img.shields.io/github/v/release/dawidchyrzynski/arduino-home-assistant?label=Version)](https://github.com/dawidchyrzynski/arduino-home-assistant/releases)
[![](https://img.shields.io/badge/Browse_Documentation-40BC13)](https://dawidchyrzynski.github.io/arduino-home-assistant/)
[![](https://img.shields.io/static/v1?label=Sponsor&message=%E2%9D%A4&logo=GitHub&color=%23fe8e86)](https://github.com/sponsors/dawidchyrzynski)

ArduinoHA allows to integrate an Arduino/ESP based device with Home Assistant using MQTT.
The library is designed to use as low resources (RAM/flash) as possible.
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

| Home Assistant type | Supported |
| ------------------- | :--------: |
| Alarm control panel |     ❌     |
| Binary sensor       |     ✅     |
| Button              |     ✅     |
| Camera              |     ✅     |
| Cover               |     ✅     |
| Device tracker      |     ✅     |
| Device trigger      |     ✅     |
| Event               |     ❌     |
| Fan                 |     ✅     |
| Humidifier          |     ❌     |
| Image               |     ❌     |
| HVAC                |     ✅     |
| Lawn mower          |     ❌     |
| Light               |     ✅     |
| Lock                |     ✅     |
| Number              |     ✅     |
| Scene               |     ✅     |
| Select              |     ✅     |
| Sensor              |     ✅     |
| Siren               |     ❌     |
| Switch              |     ✅     |
| Update              |     ❌     |
| Tag scanner         |     ✅     |
| Text                |     ❌     |
| Vacuum              |     ❌     |
| Valve               |     ❌     |
| Water heater        |     ❌     |

## Examples

|Example|Description                  |
|-------|-----------------------------|
|[Binary sensor](examples/binary-sensor/binary-sensor.ino)|Using the binary sensor as a door contact sensor.|
|[Button](examples/button/button.ino)|Adding simple buttons to the Home Assistant panel.|
|[Camera](examples/esp32-cam/esp32-cam.ino)|Publishing the preview from the ESP32-CAM module.|
|[Cover](examples/cover/cover.ino)|Controlling a window cover (open / close / stop).|
|[Device trigger](examples/multi-state-button/multi-state-button.ino)|Implementation of a simple wall switch that reports press and hold states.|
|[Fan](examples/fan/fan.ino)|Controlling a simple fan (state + speed).|
|[HVAC](examples/hvac/hvac.ino)|HVAC controller with multiple modes, power control and target temperature.|
|[Lock](examples/lock/lock.ino)|A simple door lock that's controlled by the Home Assistant.|
|[Light](examples/light/light.ino)|A simple light that allows changing brightness, color temperature and RGB color.|
|[Number](examples/number/number.ino)|Adding an interactive numeric slider in the Home Assistant panel.|
|[Scene](examples/scene/scene.ino)|Adding a custom scene in the Home Assistant panel. |
|[Select](examples/select/select.ino)|A dropdown selector that's displayed in the Home Assistant panel.|
|[Sensor](examples/sensor/sensor.ino)|A simple sensor that reports a state in a string representation (open / opening / close).|
|[Analog sensor](examples/sensor-analog/sensor-analog.ino)|Reporting the analog pin's voltage to the Home Assistant.|
|[Integer sensor](examples/sensor-integer/sensor-integer.ino)|Reporting the device's uptime to the Home Assistant.|
|[Switch](examples/led-switch/led-switch.ino)|The LED that's controlled by the Home Assistant.|
|[Multi-switch](examples/multi-switch/multi-switch.ino)|Multiple switches controlled by the Home Assistant.|
|[Tag scanner](examples/tag-scanner/tag-scanner.ino)|Scanning RFID tags using the MFRC522 module.|
|[Availability](examples/availability/availability.ino)|Reporting entities' availability (online / offline) to the Home Assistant.|
|[Advanced availability](examples/advanced-availability/advanced-availability.ino)|Advanced availability reporting with MQTT LWT (Last Will and Testament).|
|[MQTT advanced](examples/mqtt-advanced/mqtt-advanced.ino)|Subscribing to custom topics and publishing custom messages.|
|[MQTT with credentials](examples/mqtt-with-credentials/mqtt-with-credentials.ino)|Establishing connection with a MQTT broker using the credentials. |
|[NodeMCU (ESP8266)](examples/nodemcu/nodemcu.ino)|Basic example for ESP8266 devices.|
|[Arduino Nano 33 IoT](examples/nano33iot/nano33iot.ino)|Basic example for Arduino Nano 33 IoT (SAMD family).|
|[mDNS discovery](examples/mdns/mdns.ino)|Make your ESP8266 discoverable via the mDNS.|

## Compatible hardware

The library uses the Arduino Ethernet Client API for interacting with the network hardware.
It should work fine as long as the `Client` class is available.

Here is the list of devices on which the library was tested:

* Arduino Uno
* Arduino Mega
* Arduino Nano
* Arduino Pro Mini
* Arduino Nano 33 IoT
* Arduino Due
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

## FAQ

### ESP32 watchdog resets during WiFi drop / MQTT reconnect

On Arduino-ESP32, `loopTask` is registered with the Task Watchdog Timer
(TWDT) by default. If WiFi drops while your sketch is inside
`HAMqtt::loop()`, the underlying PubSubClient may block for several
seconds trying to reconnect. If that exceeds the TWDT timeout (5 s by
default on recent cores) the board resets with:

```
E (12345) task_wdt: Task watchdog got triggered. The following tasks did not reset:
E (12345) task_wdt:  - loopTask (CPU 0)
```

Pick one of the following mitigations based on your tolerance for
watchdog coverage:

**1. Gate `mqtt.loop()` on WiFi state (recommended).** Avoids entering
the blocking reconnect path entirely while WiFi is down:

```cpp
void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        mqtt.loop();
    }
    // your other work
}
```

**2. Feed the watchdog around `mqtt.loop()`.** Keeps TWDT coverage for
the rest of your code:

```cpp
#include <esp_task_wdt.h>

void loop() {
    esp_task_wdt_reset();
    mqtt.loop();
    // your other work
}
```

**3. Raise the TWDT timeout.** On Arduino-ESP32 3.x:

```cpp
#include <esp_task_wdt.h>

void setup() {
    esp_task_wdt_config_t wdt_cfg = {
        .timeout_ms = 30000,
        .idle_core_mask = 0,
        .trigger_panic = false,
    };
    esp_task_wdt_reconfigure(&wdt_cfg);
    // ... your normal setup
}
```

**4. Remove `loopTask` from the TWDT entirely (last resort — you lose
watchdog coverage for genuine hangs):**

```cpp
#include <esp_task_wdt.h>

void setup() {
    esp_task_wdt_delete(NULL);
    // ... your normal setup
}
```

If you're writing a FreeRTOS task that calls `mqtt.loop()` directly
(i.e. you're not piggy-backing on `loopTask`), register the task with
TWDT via `esp_task_wdt_add(xTaskGetCurrentTaskHandle())` and call
`esp_task_wdt_reset()` on each loop iteration.
