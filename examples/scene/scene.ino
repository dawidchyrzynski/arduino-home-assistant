#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "myScene" is unique IDs of buttons. You should define you own ID.
HAScene scene("myScene");

void onSceneCommand(HAScene* sender)
{
    if (sender == &scene) {
        // scene was activated via Home Assistant panel
    }
}

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // optional device's details
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // optional properties
    scene.setIcon("mdi:fire");
    scene.setName("Pretty Scene");

    // press callbacks
    scene.onCommand(onSceneCommand);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
}
