#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// HAFan::SpeedsFeature enables support for setting different speeds of fan.
// You can skip this argument if you don't need speed management.
// "ventilation" is unique ID of the fan. You should define your own ID.
HAFan fan("ventilation", HAFan::SpeedsFeature);

void onStateChanged(bool state) {
    Serial.print("State: ");
    Serial.println(state);
}

void onSpeedChanged(uint16_t speed) {
    Serial.print("Speed: ");
    Serial.println(speed);
}

void setup() {
    Serial.begin(9600);

    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // configure fan (optional)
    fan.setName("Bathroom");
    fan.setRetain(true);
    fan.setSpeedRangeMin(1);
    fan.setSpeedRangeMax(100);

    // handle fan states
    fan.onStateChanged(onStateChanged);
    fan.onSpeedChanged(onSpeedChanged);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
}
