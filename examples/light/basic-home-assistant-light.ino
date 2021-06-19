#include <ESP8266WiFi.h>
#include <ArduinoHA.h>

#define LED_PIN         LED_BUILTIN
#define BROKER_ADDR     IPAddress(192,168,0,1)
#define WIFI_SSID       ""
#define WIFI_PASSWORD   ""

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);
HALight led("led", false); // "led" is unique ID of the switch. You should define your own ID.

void onStateChanged(bool state)
{
    Serial.print("Got a state change request to turn: ");
    Serial.println(state ? "ON" : "OFF");
    digitalWrite(LED_PIN, (state ? HIGH : LOW));
}

void setup() {
    Serial.begin(115200);
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
    // The "Name" here is the "Device Name" in home assistant
    device.setName("My Example Home Assistant Light");
    device.setSoftwareVersion("1.0.0");

    // Register the callback so that the we know what function to call
    // when the state changes
    led.onStateChanged(onStateChanged);
    // This is the Display name of the toggle switch on the device
    // and ends up being the an entity in Home Assistant
    led.setName("My LED");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    mqtt.loop();
}
