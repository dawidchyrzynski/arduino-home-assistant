# Home Assistant availability example

This example shows how to use "availability" feature of the Home Assistant.
It's supported only by following device types:
* Binary sensor
* Sensor
* Switch
* HVACs
* Fans

## Initialization and usage

Availability feature is turned off by default. In order to turn it on for the specific
sensor/switch you need to set initial state of the unit by calling `setAvailability` method.

Example:
```cpp
...
HABinarySensor sensor("input", "door", true, mqtt);

void setup() {
    // ...

    // method must be called before `mqtt.begin(...)`
    sensor.setAvailability(false); // offline

    mqtt.begin(BROKER_ADDR);
}
...
```

After availability is set for the very first time you can call `setAvailability` in any time.
Each time the method is called, the device will publish MQTT message to the broker.
