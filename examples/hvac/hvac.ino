#include <ESP8266WiFi.h>
#include <ArduinoHA.h>

#define BROKER_ADDR     IPAddress(192,168,0,17)
#define WIFI_SSID       "MyNetwork"
#define WIFI_PASSWORD   "MyPassword"

WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

// see src/device-types/HAHVAC.h header for more details
HAHVAC hvac("my_name", HAHVAC::AuxHeatingFeature | HAHVAC::AwayModeFeature | HAHVAC::HoldFeature);

unsigned long lastTempPublishAt = 0;
double lastTemp = 0;

void onAuxHeatingStateChanged(bool state) {
    Serial.print("Aux heating: ");
    Serial.println(state);
}

void onAwayStateChanged(bool state) {
    Serial.print("Away state: ");
    Serial.println(state);
}

void onHoldStateChanged(bool state) {
    Serial.print("Hold state: ");
    Serial.println(state);
}

void onTargetTemperatureChanged(double temp) {
    Serial.print("Target temperature: ");
    Serial.println(temp);
}

void onModeChanged(HAHVAC::Mode mode) {
    Serial.print("Mode: ");
    if (mode == HAHVAC::OffMode) {
        Serial.println("off");
    } else if (mode == HAHVAC::AutoMode) {
        Serial.println("auto");
    } else if (mode == HAHVAC::CoolMode) {
        Serial.println("cool");
    } else if (mode == HAHVAC::HeatMode) {
        Serial.println("heat");
    } else if (mode == HAHVAC::DryMode) {
        Serial.println("dry");
    } else if (mode == HAHVAC::FanOnlyMode) {
        Serial.println("fan only");
    }
}

void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");

    // Unique ID must be set!
    byte mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);
    device.setUniqueId(mac, sizeof(mac));

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

    // assign callbacks (optional)
    hvac.onAuxHeatingStateChanged(onAuxHeatingStateChanged);
    hvac.onAwayStateChanged(onAwayStateChanged);
    hvac.onHoldStateChanged(onHoldStateChanged);
    hvac.onTargetTemperatureChanged(onTargetTemperatureChanged);
    hvac.onModeChanged(onModeChanged);

    // configure HVAC (optional)
    hvac.setName("My HVAC");
    hvac.setMinTemp(10);
    hvac.setMaxTemp(30);
    hvac.setTempStep(0.5);
    hvac.setRetain(true);

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    mqtt.loop();

    if ((millis() - lastTempPublishAt) > 3000) {
        hvac.setCurrentTemperature(lastTemp);
        lastTempPublishAt = millis();
        lastTemp += 0.5;
    }
}
