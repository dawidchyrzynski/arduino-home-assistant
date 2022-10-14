#include <WiFiNINA.h>
#include <ArduinoHA.h>

#define LED_PIN         9
#define BROKER_ADDR     IPAddress(192,168,0,17)
#define WIFI_SSID       "MyNetwork"
#define WIFI_PASSWORD   "MyPassword"

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

// "led" is unique ID of the switch. You should define your own ID.
HASwitch led("led");

void onSwitchCommand(bool state, HASwitch* sender)
{
    digitalWrite(LED_PIN, (state ? HIGH : LOW));
    sender->setState(state); // report state back to the Home Assistant
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");

    // Unique ID must be set!
    byte mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);
    device.setUniqueId(mac, sizeof(mac));

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // connect to wifi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500); // waiting for the connection
    }
    Serial.println();
    Serial.println("Connected to the network");

    // set device's details (optional)
    device.setName("Nano 33 IoT");
    device.setSoftwareVersion("1.0.0");

    // handle switch state
    led.onCommand(onSwitchCommand);
    led.setName("My LED"); // optional

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    mqtt.loop();

    // You can also change the state at runtime as shown below.
    // This kind of logic can be used if you want to control your switch using a button connected to the device.
    // led.setState(true); // use any state you want
}
