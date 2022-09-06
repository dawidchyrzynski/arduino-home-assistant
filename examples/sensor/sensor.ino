#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "myValve" is unique ID of the sensor. You should define your own ID.
HASensor valve("myValve");

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // configure sensor (optional)
    valve.setIcon("mdi:home");
    valve.setName("Water valve");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    // you can report different states as follows:
    // valve.setValue("open");
    // valve.setValue("opening");
    // valve.setValue("close");

    // Unlike the other device types, the HASensor doesn't store the previous value that was set.
    // It means that the MQTT message is produced each time the setValue method is called.
}
