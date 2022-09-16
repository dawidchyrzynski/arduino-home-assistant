#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
HANumber number("myNumber");

// You can also specify the precision of the number by providing the second argument to the constructor as follows:
// HANumber number("myNumber", HANumber::PrecisionP2);
// HANumber number("myNumber", HANumber::PrecisionP2);
// HANumber number("myNumber", HANumber::PrecisionP3);

void onNumberCommand(HAUtils::Number number, uint8_t precision, HANumber* s)
{
    if (number == HANumber::StateNone) {
        // the reset command was send by Home Assistant
    } else {
        // By default the "number" is integer representation of the floating point numbers.
        // For example: if precision is set to 2, the "25.12" float will be represented as "2512".
        // If you're using floats you can convert the number to float as shown below.
        // If you only need integers then conversion is not needed.
        float numberFloat = HAUtils::getFloatValue(number, precision);

        // do whatever you want with the number or numberFloat
    }

    s->setState(number); // report the selected option back to the HA panel
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
    // number.setStep(1); // minimum step: 0.001
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
    // number.setState(25.25); // remember to change precision before using floats
}