/**
 * @file      dynamic-multi-switch.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-02-22
 * @Note      Use ESP32 as a test
 * 
 */

#include <WiFi.h>
#include <ArduinoHA.h>

// Change to your actual MQTT IP address
#define BROKER_ADDR IPAddress(192,168,36,11)

const char *ssid     = "your-ssid";
const char *password = "your-password";


WiFiClient client;
HADevice device;
// 20 - 1 = 19 devices are allowed to register
HAMqtt   mqtt(client, device, 20);

void onSwitchCommand(bool state, HASwitch *sender)
{
    Serial.print(sender->getName());
    Serial.print(" Set ");
    Serial.println(state);
    sender->setState(state); // report state back to the Home Assistant
}

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    uint8_t mac[6];

    // Set the unique address of the device,ESP32 Mac address is used here
    WiFi.macAddress(mac);
    device.setUniqueId(mac, sizeof(mac));


    for (int i = 0; i <= 19; ++i) {
        // Create a device name to ensure uniqueness
        String devName = "Switch" + String(i);
        Serial.print("Create "); Serial.print(devName); Serial.println(" Object");
        HASwitch *switchPtr = new HASwitch(devName.c_str());
        switchPtr->setName(devName.c_str());
        switchPtr->setIcon("mdi:lightbulb");
        switchPtr->onCommand(onSwitchCommand);
    }


    mqtt.begin(BROKER_ADDR);
}

void loop()
{
    mqtt.loop();
}