#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR         IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

void onMqttMessage(const char* topic, const uint8_t* payload, uint16_t length) {
    Serial.print("New message on topic: ");
    Serial.println(topic);
    Serial.print("Data: ");
    Serial.println((const char*)payload);
}

void onMqttConnected() {
    Serial.println("Connected to the broker!");

    // You can subscribe to custom topic if you need
    mqtt.subscribe("myCustomTopic");
}

void onMqttConnectionFailed() {
    Serial.println("Failed to connect to the broker!");
}

void setup() {
    Serial.begin(9600);
    Ethernet.begin(mac);

    mqtt.onMessage(onMqttMessage);
    mqtt.onConnected(onMqttConnected);
    mqtt.onConnectionFailed(onMqttConnectionFailed);
    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();
}
