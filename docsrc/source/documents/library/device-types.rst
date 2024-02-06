
Device types (entities)
=======================

Device type represents a single entity within the Home Assistant panel, which could be a sensor, lock, camera, or any other item listed in the table below.

Your physical device, such as an ESP-01 board, can have multiple device types assigned to it.
These types will then appear as child entities in the Home Assistant panel.

Identifiers
-----------

Home Assistant utilizes three distinct identifiers, which might initially appear confusing.
Grasping the purpose of each is crucial for a clear understanding of the library's API.

Entity ID
^^^^^^^^^

Home Assistant automatically generates an entity ID for each device type registered by your device.
This ID is primarily utilized by dashboards and automations within Home Assistant.

When the entity is discovered by Home Assistant for the first time, the name field is automatically employed to generate the entity ID.
Once registered, you can modify it to any desired value using the Home Assistant User Interface.

Home Assistant internally relies on the unique ID, so changing the entity ID or name in the Home Assistant UI does not break the integration between your device and Home Assistant.

Object ID
^^^^^^^^^

The object ID is an optional identifier that you can assign to the device type.
Its sole purpose is for generating the entity ID described above.

By default, Home Assistant generates the entity ID based on the entity's name.
However, when the object ID is provided, Home Assistant uses it to generate the entity ID.

Consequently, you can use the entity's name as a user-friendly label and the object ID as an internal identifier.

Unique ID
^^^^^^^^^

The unique ID serves as an internal identifier for the entity within the Home Assistant instance.
Once the entity with a specific unique ID is created, it cannot be altered, as this identifier is not accessible through the user interface.

Home Assistant utilizes this identifier internally to store the parameters of the entity in the database.
Given that the unique ID must be unique across the entire Home Assistant instance.
Multiple devices cannot expose entities with the same unique ID.

By default, the library uses the unique ID provided in the device type's constructor.
However, when you reuse the same codebase on multiple devices, conflicts may arise.
To address this issue, you can enable the extended unique ID feature in the HADevice instance.
This feature incorporates the device's unique ID as a prefix for the device type's ID.

.. code-block:: cpp
    :caption: Default behavior

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

    EthernetClient client;
    HADevice device(mac, sizeof(mac)); // the unique ID of the device will be 0010fa6e384a
    HAMqtt mqtt(client, device);

    // "myValve" is unique ID of the sensor. You should define your own ID.
    HASensor valve("myValve");

    void setup() {
        // ...

        valve.setIcon("mdi:home");
        valve.setName("Water valve");

        // the unique ID of the valve in HA will be "myValve"

        // ...
    }

    void loop() {
        // ...
    }

.. code-block:: cpp
    :caption: Extended unique IDs

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

    EthernetClient client;
    HADevice device(mac, sizeof(mac)); // the unique ID of the device will be 0010fa6e384a
    HAMqtt mqtt(client, device);

    // "myValve" is unique ID of the sensor. You should define your own ID.
    HASensor valve("myValve");

    void setup() {
        // ...

        device.enableExtendedUniqueIds(); // <------------ enables extended unique IDs
        valve.setIcon("mdi:home");
        valve.setName("Water valve");

        // the unique ID of the valve in HA will be "0010fa6e384a_myValve"

        // ...
    }

    void loop() {
        // ...
    }

Limitations
-----------

Registering a new device type involves utilizing a certain amount of flash and RAM memory.
On less powerful units, such as the Arduino Uno, you may rapidly reach the resource limit.
Therefore, it is advisable to keep the device simple to avoid hitting the resource limit, which could lead to random reboots of the device.

By default, devices utilizing ATmega328P or ATmega168 processors are limited to a maximum of 6 device types, while other platforms can support up to 24 device types.
You can increase the limit using the :doc:`HAMqtt </documents/api/core/ha-mqtt>` class constructor as follows:

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device, 40); // <------------ 40 is a new limit of device types

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
   * - Alarm control panel
     - ❌
     - --
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
   * - Event
     - ❌
     - --
   * - Fan
     - ✅
     - :doc:`HAFan </documents/api/device-types/ha-fan>`
   * - Humidifier
     - ❌
     - --
   * - HVAC
     - ✅
     - :doc:`HAHVAC </documents/api/device-types/ha-hvac>`
   * - Lawn mower
     - ❌
     - --
   * - Light
     - ✅
     - :doc:`HALight </documents/api/device-types/ha-light>`
   * - Lock
     - ✅
     - :doc:`HALock </documents/api/device-types/ha-lock>`
   * - Number
     - ✅
     - :doc:`HANumber </documents/api/device-types/ha-number>`
   * - Scene
     - ✅
     - :doc:`HAScene </documents/api/device-types/ha-scene>`
   * - Select
     - ✅
     - :doc:`HASelect </documents/api/device-types/ha-select>`
   * - | Sensor (text)
     - ✅
     - :doc:`HASensor </documents/api/device-types/ha-sensor>`
   * - | Sensor (number)
     - ✅
     - :doc:`HASensorNumber </documents/api/device-types/ha-sensor-number>`
   * - Siren
     - ❌
     - --
   * - Switch
     - ✅
     - :doc:`HASwitch </documents/api/device-types/ha-switch>`
   * - Update
     - ❌
     - --
   * - Tag scanner
     - ✅
     - :doc:`HATagScanner </documents/api/device-types/ha-tag-scanner>`
   * - Text
     - ❌
     - --
   * - Vacuum
     - ❌
     - --
   * - Valve
     - ❌
     - --
   * - Water heater
     - ❌
     - --
