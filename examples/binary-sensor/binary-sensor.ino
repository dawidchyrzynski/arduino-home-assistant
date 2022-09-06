#include <Ethernet.h>
#include <ArduinoHA.h>

#define INPUT_PIN       9
#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
unsigned long lastReadAt = millis();
bool lastInputState = false;

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "myInput" is unique ID of the sensor. You should define you own ID.
HABinarySensor sensor("myInput");

void setup() {
    pinMode(INPUT_PIN, INPUT_PULLUP);
    lastInputState = digitalRead(INPUT_PIN);

    // you don't need to verify return status
    Ethernet.begin(mac);

    // optional device's details
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // optional properties
    sensor.setCurrentState(lastInputState);
    sensor.setName("Door sensor");
    sensor.setDeviceClass("door");
    sensor.setIcon("mdi:fire");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    // reading the digital input of the Arduino device
    if ((millis() - lastReadAt) > 30) { // read in 30ms interval
        // library produces MQTT message if a new state is different than the previous one
        sensor.setState(digitalRead(INPUT_PIN));
        lastInputState = sensor.getCurrentState();
        lastReadAt = millis();
    }
}
