Introduction
============

Prior to implementing the business logic of your application, you'll need to incorporate several essential pieces of code into your project.
Essentially, everything boils down to a few fundamental rules:

1) :doc:`HADevice </documents/api/core/ha-device>` and :doc:`HAMqtt </documents/api/core/ha-mqtt>` instances need to be initialized once globally or as a part of another global object.
2) :doc:`HAMqtt::begin </documents/api/core/ha-mqtt>` needs to be called at the end of Arduino's setup logic. It lets you provide the MQTT broker's IP address and credentials.
3) :doc:`HAMqtt::loop </documents/api/core/ha-mqtt>` method needs to be called periodically (it doesn't need to be called on each tick).
4) Device types need to be initialized after :doc:`HAMqtt </documents/api/core/ha-mqtt>` class (it will be described later in the documentation).

Here are the minimal boilerplate examples to get you started.
Don't worry if you have no idea what's happening here; everything will be explained in detail in the following chapters.

Arduino Boilerplate
-------------------

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device);

    void setup() {
        Ethernet.begin(mac);

        // your setup logic goes here

        // MQTT broker connection (use your data here)
        mqtt.begin("192.168.1.50", "username", "password");
    }

    void loop() {
        Ethernet.maintain();
        mqtt.loop();

        // your loop logic goes here
    }

ESP32/ESP8266 Boilerplate
-------------------------

::

    #include <ESP8266WiFi.h>
    #include <ArduinoHA.h>

    WiFiClient client;
    HADevice device;
    HAMqtt mqtt(client, device);

    void setup() {
        byte mac[WL_MAC_ADDR_LENGTH];
        WiFi.macAddress(mac);
        device.setUniqueId(mac, sizeof(mac));

        // you can skip this part if you're already maintaining the connection logic
        WiFi.begin("MyNetworkSSID", "MyPassword");
        while (WiFi.status() != WL_CONNECTED) {
            delay(500); // waiting for the connection
        }

        // your setup logic goes here

        // MQTT broker connection (use your data here)
        mqtt.begin("192.168.1.50", "username", "password");
    }

    void loop() {
        mqtt.loop();

        // your loop logic goes here
    }