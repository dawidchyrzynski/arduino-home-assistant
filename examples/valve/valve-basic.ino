#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "myValve" is unique ID of the valve. You should define your own ID.
HAValve valve("myValve");

// HA will not send state stopped unless device advertises feature compatibility
// HAValve valve("myValve", HAValve::StopFeature);

// For basic operation, HA will send named states to the state topic
// See https://www.home-assistant.io/integrations/valve.mqtt/

void onValveCommand(int16_t cmd, HAValve* sender) {
    if (cmd == HAValve::CommandOpen) {
        Serial.println("Command: Open");
        sender->setState(HAValve::StateOpening); // report state back to the HA
    } else if (cmd == HAValve::CommandClose) {
        Serial.println("Command: Close");
        sender->setState(HAValve::StateClosing); // report state back to the HA
    } else if (cmd == HAValve::CommandStop) {
        Serial.println("Command: Stop");
        sender->setState(HAValve::StateClosed); // valve cannot report state stopped!
    }

    // Available states:
    // HAValve::StateClosed
    // HAValve::StateClosing
    // HAValve::StateOpen
    // HAValve::StateOpening
}

void setup() {
    Serial.begin(9600);
    Ethernet.begin(mac);

    // optional device's details
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    valve.onCommand(onValveCommand);
    valve.setName("My valve"); // optional

    // Optionally you can set the device class
    // see https://www.home-assistant.io/integrations/valve/#device-class
    // valve.setDeviceClass("water");
    
    // Optionally you can set retain flag for the HA commands
    // valve.setRetain(true);

    // Optionally you can enable optimistic mode for the HAValve.
    // In this mode you won't need to report state back to the HA when commands are executed.
    // valve.setOptimistic(true);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    // You can also change the state at runtime as shown below.
    // This kind of logic can be used if you want to control your valve using a button connected to the device.
    // valve.setState(HAValve::StateOpening); // use any state you want
}
