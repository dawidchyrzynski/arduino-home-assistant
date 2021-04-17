#include <Ethernet.h>
#include <ArduinoHA.h>

#define INPUT_PIN       9
#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
unsigned long lastReadAt = millis();
unsigned long lastAvailabilityToggleAt = millis();
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

    lastReadAt = millis();
    lastAvailabilityToggleAt = millis();

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    sensor.setName("Door sensor"); // optional

    // This method enables availability for all device types registered on the device.
    // For example, if you have 5 sensors on the same device, you can enable
    // shared availability and change availability state of all sensors using
    // single method call "device.setAvailability(false|true)"
    device.enableSharedAvailability();

    // Optionally, you can enable MQTT LWT feature. If device will lose connection
    // to the broker, all device types related to it will be marked as offline in
    // the Home Assistant Panel.
    device.enableLastWill();

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

    if ((millis() - lastAvailabilityToggleAt) > 5000) {
        device.setAvailability(!device.isOnline());
        lastAvailabilityToggleAt = millis();
    }
}
