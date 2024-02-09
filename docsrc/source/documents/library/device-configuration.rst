Device configuration
====================

:doc:`HADevice </documents/api/core/ha-device>` represents the physical device on which the library is used.
Essentially, it's a group of types such as sensors, switches, lights, and more.
Within Home Assistant, it appears with properties that can be configured using the library's API.

Every property, except for the unique ID, is optional.
Enabling optional properties can lead to increased flash and RAM usage, therefore it is not advisable to set them on lower-spec MCUs.

The supported properties are:

* unique ID*
* name
* software version
* manufacturer
* model
* configuration url

Unique ID
---------

The unique ID serves as an internal identifier for devices within Home Assistant.
With this ID, Home Assistant can monitor the device's parameters and the entities it exposes.
The unique ID must be distinct within the scope of the Home Assistant instance.
The recommended approach is to use the MAC address of an Ethernet or Wi-Fi chip.

There are three distinct methods for setting the device ID, allowing you to choose the one that best suits your requirements.

1) Providing string (const char*) to the :doc:`HADevice </documents/api/core/ha-device>` constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Try to keep the ID simple (alphanumeric characters) and short.

::

    #include <ArduinoHA.h>

    HADevice device("myUniqueID");

    void setup() {
        // ...
    }

    void loop() {
        // ...
    }

2) Providing byte array to the :doc:`HADevice </documents/api/core/ha-device>` constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    #include <ArduinoHA.h>

    // use your own unique bytes sequence
    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    HADevice device(mac, sizeof(mac));

    void setup() {
        // ...
    }

    void loop() {
        // ...
    }

3) Using :doc:`HADevice::setUniqueId </documents/api/core/ha-device>` method during the setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    #include <ArduinoHA.h>

    HADevice device;

    void setup() {
        // any length is acceptable
        byte myId[] = {0x05, 0xb4, 0xc6, 0x9f, 0xbe, 0xce, 0x8c, 0x1f, 0xc7};
        device.setUniqueId(myId, sizeof(myId)); // the input array is cloned internally

        // ...
    }

    void loop() {
        // ...
    }

Device properties
-----------------

Each property has its corresponding setter method in the :doc:`HADevice </documents/api/core/ha-device>` class.
Please note that all of these methods accept a const char pointer whose **content is not copied**.

::

    #include <ArduinoHA.h>

    HADevice device("myUniqueId");

    void setup() {
        device.setName("Bedroom Light Controller");
        device.setSoftwareVersion("1.0.0");
        device.setManufacturer("Developer Corp.");
        device.setModel("ABC-123");
        device.setConfigurationUrl("http://192.168.1.55:1234");

        // ...
    }

    void loop() {
        // ...
    }