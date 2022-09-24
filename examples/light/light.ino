#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

// HALight::BrightnessFeature enables support for setting brightness of the light.
// You can skip this argument if you don't need brightness management.
// "prettyLight" is unique ID of the light. You should define your own ID.
HALight light("prettyLight", HALight::BrightnessFeature);

void onStateCommand(bool state, HALight* light) {
    Serial.print("State: ");
    Serial.println(state);

    light->setState(state); // report state back to the Home Assistant
}

void onBrightnessCommand(uint8_t brightness, HALight* light) {
    Serial.print("Brightness: ");
    Serial.println(brightness);

    light->setBrightness(brightness); // report brightness back to the Home Assistant
}

void setup() {
    Serial.begin(9600);

    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // configure light (optional)
    light.setName("Bathroom");

    // Optionally you can set retain flag for the HA commands
    // light.setRetain(true);

    // Maximum brightness level can be changed as follows:
    // light.setBrightnessScale(50);

    // Optionally you can enable optimistic mode for the HALight.
    // In this mode you won't need to report state back to the HA when commands are executed.
    // light.setOptimistic(true);

    // handle light states
    light.onStateCommand(onStateCommand);
    light.onBrightnessCommand(onBrightnessCommand);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    // You can also change the state at runtime as shown below.
    // This kind of logic can be used if you want to control your light using a button connected to the device.
    // light.setState(true); // use any state you want
}