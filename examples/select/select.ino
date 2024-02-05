#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
HASelect mySelect("mySelect");

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

    // it may return null
    if (sender->getCurrentOption()) {
        Serial.print("Current option: ");
        Serial.println(sender->getCurrentOption());
    }
}

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // set available options
    mySelect.setOptions("Low;Medium;High"); // use semicolons as separator of options
    mySelect.onCommand(onSelectCommand);

    mySelect.setIcon("mdi:home"); // optional
    mySelect.setName("My dropdown"); // optional

    // Optionally you can set retain flag for the HA commands
    // mySelect.setRetain(true);

    // Optionally you can enable optimistic mode for the HASelect.
    // In this mode you won't need to report state back to the HA when commands are executed.
    // mySelect.setOptimistic(true);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    // You can also report the state to the HA panel at runtime as shown below.
    // The integer corresponds to the option's index.
    // mySelect.setState(1);

    // You can also reset the select as follows:
    // mySelect.setState(-1);
}