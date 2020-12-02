#include <Ethernet.h>

#include "src/ArduinoHA.h"

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

EthernetClient client;
HADevice device("unique-id");
HAMqtt mqtt("client-id", client, device);
HATriggers triggers(mqtt);

uint32_t lastTriggerAt = 0;

void setup() {
    Serial.begin(9600);
    Ethernet.begin(mac);

    Serial.println("Got IP");
    Serial.println(Ethernet.localIP());

    //device.setManufacturer("Arduino");
    //device.setModel("Arduino Uno");
    device.setName("My arduino");
    device.setSoftwareVersion("1.0.0");

    // triggers setup
    triggers.add("press", "s1");
    triggers.add("hold", "s1");

    // switch setup

    mqtt.begin(IPAddress(192,168,0,17));
}

void loop() {
    mqtt.loop();

    if ((millis() - lastTriggerAt) >= 3000) {
        triggers.trigger("press", "s1");
        lastTriggerAt = millis();
    }
}
