#include <Ethernet.h>
#include <ArduinoHA.h>

#define LED_PIN         9
#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
HASwitch led("led", false); // you can use custom name in place of "led"

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

    // handle switch state
    led.onStateChanged(onSwitchStateChanged);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
}
