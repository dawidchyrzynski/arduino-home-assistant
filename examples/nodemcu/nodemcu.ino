#include <ESP8266WiFi.h>
#include <ArduinoHA.h>

#define LED_PIN         D0
#define BROKER_ADDR     IPAddress(192,168,0,17)
#define WIFI_SSID       "MyNetwork"
#define WIFI_PASSWORD   "MyPassword"

byte mac[6];
WiFiClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
HASwitch led("led", false, mqtt); // you can use custom name in place of "led"

void onSwitchStateChanged(bool state, HASwitch* s)
{
    digitalWrite(LED_PIN, (state ? HIGH : LOW));
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");

    WiFi.macAddress(mac);
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
    led.onStateChanged(onSwitchStateChanged);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    mqtt.loop();
}
