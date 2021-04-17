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

// "input" is unique ID of the sensor. You should define you own ID.
// "door" is device class (based on the class HA displays different icons in the panel)
// "true" is initial state of the sensor. In this example it's "true" as we use pullup resistor
HABinarySensor sensor("input", "door", true);

void setup() {
    pinMode(INPUT_PIN, INPUT_PULLUP);
    lastInputState = digitalRead(INPUT_PIN);

    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    sensor.setName("Door sensor"); // optional

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    if ((millis() - lastReadAt) > 30) { // read in 30ms interval
        // library produces MQTT message if a new state is different than the previous one
        sensor.setState(digitalRead(INPUT_PIN));
        lastInputState = sensor.getState();
        lastReadAt = millis();
    }
}
