MQTT security
=============

The library enables you to utilize credentials to establish a TCP connection with the MQTT broker.
By default, you can employ the same credentials utilized for logging into the Home Assistant panel.
However, it's also possible to configure custom credentials within the Mosquitto broker.

.. DANGER::

    This solution may not provide comprehensive security as the communication between Arduino and Home Assistant lacks encryption.
    Consequently, username and password could be vulnerable to discovery through analysis of local network traffic.
    However, this may not pose a significant risk if your local network is properly secured against unauthorized access.

::

    #include <Ethernet.h>
    #include <ArduinoHA.h>

    byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
    EthernetClient client;
    HADevice device(mac, sizeof(mac));
    HAMqtt mqtt(client, device);

    void setup() {
        // ...

        // replace username and password with your credentials
        mqtt.begin("192.168.1.50", "username", "password");
    }

    void loop() {
        // ...
    }
