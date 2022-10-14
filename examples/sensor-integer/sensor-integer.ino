#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
unsigned long lastUpdateAt = 0;

// "myUptime" is unique ID of the sensor. You should define your own ID.
HASensorNumber uptimeSensor("myUptime");

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // configure sensor (optional)
    uptimeSensor.setIcon("mdi:home");
    uptimeSensor.setName("Uptime");
    uptimeSensor.setUnitOfMeasurement("s");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    if ((millis() - lastUpdateAt) > 2000) { // update in 2s interval
        unsigned long uptimeValue = millis() / 1000;
        uptimeSensor.setValue(uptimeValue);
        lastUpdateAt = millis();
    }
}
