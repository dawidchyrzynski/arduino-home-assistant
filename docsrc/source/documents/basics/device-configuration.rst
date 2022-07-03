Device configuration
=============

``HADevice`` represents the physical device where the library is installed.
Logically it's a group of types like sensors, switches, lights and so on. 
In the Home Assistant, it's listed with properties that may be configured using the library's API.

Each property except the unique ID is optional.
Setting optional properties increases flash and RAM usage so it's not recommended to set them on lower-spec MCUs.

The supported properties are:

* unique ID*
* name
* software version
* manufacturer
* model

Unique ID
---------

The ID of a device needs to be unique in a scope of a Home Assistant instance.
The safest solution is to use the MAC address of an Ethernet or Wi-Fi chip but you can also implement your own solution.

There are three different ways to set the ID of the device.
You can pick one depending on your needs.

1) Providing string (const char*) to the ``HADevice`` constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    HADevice device("myUniqueID");

    void setup() {
        // ...
    }

    void loop() {
        // ...
    }

2) Providing byte array to the ``HADevice`` constructor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    // use your own unique bytes sequence
    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    HADevice device(mac, sizeof(mac));

    void setup() {
        // ...
    }

    void loop() {
        // ...
    }

3) Using ``HADevice::setUniqueId`` method during the setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

::

    HADevice device;

    void setup() {
        // any length is acceptable
        byte myId[] = {0x05, 0xb4, 0xc6, 0x9f, 0xbe, 0xce, 0x8c, 0x1f, 0xc7};
        device.setUniqueId(myId, sizeof(myId));

        // ...
    }

    void loop() {
        // ...
    }

Device properties
-----------------

Each property has its corresponding setter method in the ``HADevice`` class.
Please note that all these methods accept const char pointer whose content is not copied.

::

    HADevice device("myUniqueId");

    void setup() {
        device.setName("Bedroom Light Controller");
        device.setSoftwareVersion("1.0.0");
        device.setManufacturer("Developer Corp.");
        device.setModel("ABC-123");

        // ...
    }

    void loop() {
        // ...
    }