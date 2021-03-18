#include <Ethernet.h>
#include <ArduinoHA.h>

#define BROKER_ADDR IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};
unsigned long lastSentAt = millis();
double lastValue = 0;

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);

/**
 * Supported data types:
 * - uint8_t
 * - uint16_t
 * - uint32_t
 * - int8_t
 * - int16_t
 * - int32_t
 * - double
 * - float
 */
// you can use custom name in place of "temp"
// "0" is initial value of the sensor
HASensor<double> temp("temp", 0);

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // you can set custom units for the sensor (optional)
    temp.setUnitOfMeasurement("°C");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    if ((millis() - lastSentAt) >= 5000) {
        lastSentAt = millis();
        lastValue = lastValue + 0.5;
        temp.setValue(lastValue);
    }
}
