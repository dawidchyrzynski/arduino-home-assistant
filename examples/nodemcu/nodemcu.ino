#include <ESP8266WiFi.h>
#include <ArduinoHA.h>

#define LED_PIN         D0
#define BROKER_ADDR     IPAddress(192,168,0,17)
#define WIFI_SSID       "MyNetwork"
#define WIFI_PASSWORD   "MyPassword"

WiFiClient client;
HADevice device;
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
    device.setName("NodeMCU");
    device.setSoftwareVersion("1.0.0");

    // handle switch state
    led.onBeforeStateChanged(onBeforeSwitchStateChanged); // optional
    led.onStateChanged(onSwitchStateChanged);
    led.setName("My LED"); // optional

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    mqtt.loop();
}
