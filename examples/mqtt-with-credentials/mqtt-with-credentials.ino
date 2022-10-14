#include <Ethernet.h>
#include <ArduinoHA.h>

#define LED_PIN             9
#define BROKER_ADDR         IPAddress(192,168,0,17)
#define BROKER_USERNAME     "user" // replace with your credentials
#define BROKER_PASSWORD     "pass"

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// "led" is unique ID of the switch. You should define your own ID.
HASwitch led("led");

void onSwitchCommand(bool state, HASwitch* sender)
{
    digitalWrite(LED_PIN, (state ? HIGH : LOW));
    sender->setState(state); // report state back to the Home Assistant
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // handle switch state
    led.onCommand(onSwitchCommand);
    led.setName("My LED"); // optional

    mqtt.begin(BROKER_ADDR, BROKER_USERNAME, BROKER_PASSWORD);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
}
