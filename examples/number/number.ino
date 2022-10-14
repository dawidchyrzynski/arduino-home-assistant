#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
HANumber number("myNumber");

// You can also specify the precision of the number by providing the second argument to the constructor as follows:
// HANumber number("myNumber", HANumber::PrecisionP1);
// HANumber number("myNumber", HANumber::PrecisionP2);
// HANumber number("myNumber", HANumber::PrecisionP3);

void onNumberCommand(HANumeric number, HANumber* sender)
{
    if (!number.isSet()) {
        // the reset command was send by Home Assistant
    } else {
        // you can do whatever you want with the number as follows:
        int8_t numberInt8 = number.toInt8();
        int16_t numberInt16 = number.toInt16();
        int32_t numberInt32 = number.toInt32();
        uint8_t numberUInt8 = number.toUInt8();
        uint16_t numberUInt16 = number.toUInt16();
        uint32_t numberUInt32 = number.toUInt32();
        float numberFloat = number.toFloat();
    }

    sender->setState(number); // report the selected option back to the HA panel
}

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // handle command from the HA panel
    number.onCommand(onNumberCommand);

    // Optional configuration
    number.setIcon("mdi:home");
    number.setName("My number");
    // number.setMin(1); // can be float if precision is set via the constructor
    // number.setMax(100); // can be float if precision is set via the constructor
    // number.setStep(0.5f); // minimum step: 0.001f
    // number.setMode(HANumber::ModeBox);
    // number.setMode(HANumber::ModeSlider);

    // You can set retain flag for the HA commands
    // number.setRetain(true);

    // You can also enable optimistic mode for the HASelect.
    // In this mode you won't need to report state back to the HA when commands are executed.
    // number.setOptimistic(true);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    // You can also report the state to the HA panel at runtime as shown below.
    // number.setState(1);
    // number.setState(25.25f); // remember to change precision before using floats
}