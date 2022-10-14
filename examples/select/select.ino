#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
HASelect select("mySelect");

void onSelectCommand(int8_t index, HASelect* sender)
{
    switch (index) {
    case 0:
        // Option "Low" was selected
        break;

    case 1:
        // Option "Medium" was selected
        break;

    case 2:
        // Option "High" was selected
        break;

    default:
        // unknown option
        return;
    }

    sender->setState(index); // report the selected option back to the HA panel
}

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // set available options
    select.setOptions("Low;Medium;High"); // use semicolons as separator of options
    select.onCommand(onSelectCommand);

    select.setIcon("mdi:home"); // optional
    select.setName("My dropdown"); // optional

    // Optionally you can set retain flag for the HA commands
    // select.setRetain(true);

    // Optionally you can enable optimistic mode for the HASelect.
    // In this mode you won't need to report state back to the HA when commands are executed.
    // select.setOptimistic(true);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    // You can also report the state to the HA panel at runtime as shown below.
    // The integer corresponds to the option's index.
    // select.setState(1);
}