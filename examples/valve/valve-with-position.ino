#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "myValve" is unique ID of the valve. You should define your own ID.
HAValve valve("myValve", HAValve::PositionFeature);

// When using PositionFeature is optional, HA will only send a numeric state
// The value is between "positionOpen" and "positionClosed"
// See https://www.home-assistant.io/integrations/valve.mqtt/

void onValveCommand(int16_t cmd, HAValve* sender) {
    if (cmd == valve.getPositionOpen()) {
        Serial.println("Command: Open");
        sender->setState(HAValve::StateOpening); // report state back to the HA
    } else if (cmd == valve.getPositionClosed()) {
        Serial.println("Command: Close");
        sender->setState(HAValve::StateClosing); // report state back to the HA
    } else {
        Serial.print("Position: "); Serial.println(cmd);
        sender->setPosition(cmd); // report position back to the HA
    }

    // Available states:
    // HAValve::StateClosed
    // HAValve::StateClosing
    // HAValve::StateOpen
    // HAValve::StateOpening

    // You can also report position using setPosition() method
    // However, for the position feature, HA supports sending both in one message
    // It may advisable to use this method instead
    // sender->setStateWithPosition(HAValve::StateOpening, 42);
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
    
    // Optionally define the open/closed values (default 0-100)
    // valve.setPositionClosed(16);
    // valve.setPositionOpen(1024);
    
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
