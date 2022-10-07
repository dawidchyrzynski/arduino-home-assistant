#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "myButtonA" and "myButtonB" are unique IDs of buttons. You should define you own ID.
HAButton buttonA("myButtonA");
HAButton buttonB("myButtonB");

void onButtonCommand(HAButton* sender)
{
    if (sender == &buttonA) {
        // button A was clicked, do your logic here
    } else if (sender == &buttonB) {
        // button B was clicked, do your logic here
    }
}

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // optional device's details
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // optional properties
    buttonA.setIcon("mdi:fire");
    buttonA.setName("Click me A");
    buttonB.setIcon("mdi:home");
    buttonB.setName("Click me B");

    // press callbacks
    buttonA.onCommand(onButtonCommand);
    buttonB.onCommand(onButtonCommand);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
}
