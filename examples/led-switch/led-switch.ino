#include <Ethernet.h>
#include <ArduinoHA.h>

#define LED_PIN         9
#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
HASwitch led("led", false); // "led" is unique ID of the switch. You should define your own ID.

void onBeforeSwitchStateChanged(bool state, HASwitch* s)
{
    // this callback will be called before publishing new state to HA
    // in some cases there may be delay before onStateChanged is called due to network latency
}

void onSwitchStateChanged(bool state, HASwitch* s)
{
    digitalWrite(LED_PIN, (state ? HIGH : LOW));
}

void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // set icon (optional)
    led.setIcon("mdi:lightbulb");
    led.setName("My LED");

    // handle switch state
    led.onBeforeStateChanged(onBeforeSwitchStateChanged); // optional
    led.onStateChanged(onSwitchStateChanged);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
}
