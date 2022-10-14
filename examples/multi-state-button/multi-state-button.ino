#include <Ethernet.h>
#include <ArduinoHA.h>

// This example uses JC Button library
// https://github.com/JChristensen/JC_Button
#include <JC_Button.h>

#define BUTTON_PIN      9
#define BUTTON_NAME     "mybtn"
#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

HADeviceTrigger shortPressTrigger(HADeviceTrigger::ButtonShortPressType, BUTTON_NAME);
HADeviceTrigger longPressTrigger(HADeviceTrigger::ButtonLongPressType, BUTTON_NAME);
Button btn(BUTTON_PIN);
bool holdingBtn = false;

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // setup JC button
    btn.begin();

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
    btn.read();

    if (btn.pressedFor(3000) && !holdingBtn) {
        longPressTrigger.trigger();
        holdingBtn = true;
    } else if (btn.wasReleased()) {
        if (holdingBtn) {
            holdingBtn = false;
        } else {
            shortPressTrigger.trigger();
        }
    }
}
