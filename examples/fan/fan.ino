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

void onStateCommand(bool state, HAFan* sender) {
    Serial.print("State: ");
    Serial.println(state);

    sender->setState(state); // report state back to the Home Assistant
}

void onSpeedCommand(uint16_t speed, HAFan* sender) {
    Serial.print("Speed: ");
    Serial.println(speed);

    sender->setSpeed(speed); // report speed back to the Home Assistant
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
    fan.setSpeedRangeMin(1);
    fan.setSpeedRangeMax(100);

    // Optionally you can set retain flag for the HA commands
    // fan.setRetain(true);

    // Optionally you can enable optimistic mode for the HAFan.
    // In this mode you won't need to report state back to the HA when commands are executed.
    // fan.setOptimistic(true);

    // handle fan states
    fan.onStateCommand(onStateCommand);
    fan.onSpeedCommand(onSpeedCommand);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    // You can also change the state at runtime as shown below.
    // This kind of logic can be used if you want to control your fan using a button connected to the device.
    // fan.setState(true); // true (ON) or false (OFF)
}