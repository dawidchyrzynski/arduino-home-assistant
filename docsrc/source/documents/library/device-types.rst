
Device types
============

Device type represents a single entity in the Home Assistant panel.
It can be a sensor, lock, camera or anything that's listed in the table below.

Your physical device (for example ESP-01 board) can have multiple device types assigned.
They will be displayed as child entities in the HA panel.

Limitations
-----------

Registering a new device type requires some flash and RAM memory to be utilized.
On less powerful units like Arduino Uno, you may quickly hit the limit of resources, so keeping the device simple is recommended.
Hitting the resource limit will result in random reboots of the device.

By default, the maximum number of device types is 6.
You can increase the limit using the :doc:`HAMqtt </documents/api/core/ha-mqtt>` class constructor as follows:

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device, 12); // <------------ 12 is a new limit of device types

    void setup() {
        Ethernet.begin(mac);

        // your setup logic goes here
    }

    void loop() {
        Ethernet.maintain();
        mqtt.loop();

        // your loop logic goes here
    }

Please note that this limit cannot be changed at runtime.

Supported device types
----------------------

.. list-table::
   :widths: 25 15 60
   :header-rows: 1
   :class: supported-device-types-table

   * - Device type
     - Supported
     - Documentation
   * - Binary sensor
     - ✅
     - :doc:`HABinarySensor </documents/api/device-types/ha-binary-sensor>`
   * - Button
     - ✅
     - :doc:`HAButton </documents/api/device-types/ha-button>`
   * - Camera
     - ✅
     - :doc:`HACamera </documents/api/device-types/ha-camera>`
   * - Cover
     - ✅
     - :doc:`HACover </documents/api/device-types/ha-cover>`
   * - Device tracker
     - ✅
     - :doc:`HADeviceTracker </documents/api/device-types/ha-device-tracker>`
   * - Device trigger
     - ✅
     - :doc:`HADeviceTrigger </documents/api/device-types/ha-device-trigger>`
   * - Fan
     - ❌
     - --
   * - Humidifier
     - ❌
     - --
   * - HVAC
     - ❌
     - --
   * - Light
     - ❌
     - --
   * - Lock
     - ✅
     - :doc:`HALock </documents/api/device-types/ha-lock>`
   * - Number
     - ✅
     - :doc:`HANumber </documents/api/device-types/ha-number>`
   * - Scene
     - ❌
     - --
   * - Select
     - ✅
     - :doc:`HASelect </documents/api/device-types/ha-select>`
   * - | Sensor
       | (text)
     - ✅
     - :doc:`HASensor </documents/api/device-types/ha-sensor>`
   * - | Sensor
       | (decimal number)
     - ✅
     - :doc:`HASensorFloat </documents/api/device-types/ha-sensor-float>`
   * - | Sensor
       | (integer number)
     - ✅
     - :doc:`HASensorInteger </documents/api/device-types/ha-sensor-integer>`
   * - Switch
     - ✅
     - :doc:`HASwitch </documents/api/device-types/ha-switch>`
   * - Tag scanner
     - ✅
     - :doc:`HATagScanner </documents/api/device-types/ha-tag-scanner>`
   * - Vacuum
     - ❌
     - --

SensorFloat vs SensorInteger
----------------------------

As you may notice, two sensor classes are available to handle numeric sensors.
The `SensorFloat` allows publishing floating point numbers to the HA panel
whereas the `SensorInteger` allows publishing only integer values. 

The `SensorFloat` can also be used to publish integer values if you set its precision to P0
but has a bigger resource overhead compared to the `HASensorInteger`.

In most cases, you should use `HASensorFloat` to publish decimal or integer numbers.
If you're limited by flash or RAM and you only need integer numbers then the `HASensorInteger` should be your choice.
