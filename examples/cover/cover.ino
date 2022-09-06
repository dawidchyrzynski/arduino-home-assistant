#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "myCover" is unique ID of the cover. You should define your own ID.
HACover cover("myCover");

void onCoverCommand(HACover::CoverCommand cmd) {
    if (cmd == HACover::CommandOpen) {
        Serial.println("Command: Open");
        cover.setState(HACover::StateOpening); // report state back to the HA
    } else if (cmd == HACover::CommandClose) {
        Serial.println("Command: Close");
        cover.setState(HACover::StateClosing); // report state back to the HA
    } else if (cmd == HACover::CommandStop) {
        Serial.println("Command: Stop");
        cover.setState(HACover::StateStopped); // report state back to the HA
    }

    // Available states:
    // HACover::StateClosed
    // HACover::StateClosing
    // HACover::StateOpen
    // HACover::StateOpening
    // HACover::StateStopped

    // You can also report position using setPosition() method
}

void setup() {
    Serial.begin(9600);
    Ethernet.begin(mac);

    cover.onCommand(onCoverCommand);
    cover.setName("My cover"); // optional

    // optionally you can set retain flag for the HA commands
    // cover.setRetain(true);

    // optionally you can enable optimistic mode for the HACover.
    // in this mode you won't need to report state back to the HA when a commands are executed
    // cover.setOptimistic(true);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
}
