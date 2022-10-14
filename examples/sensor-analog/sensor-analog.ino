#include <Ethernet.h>
#include <ArduinoHA.h>

#define ANALOG_PIN      A0
#define BROKER_ADDR     IPAddress(192,168,0,17)

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x4A};

EthernetClient client;
HADevice device(mac, sizeof(mac));
HAMqtt mqtt(client, device);
unsigned long lastUpdateAt = 0;

// "myAnalogInput" is unique ID of the sensor. You should define your own ID.
HASensorNumber analogSensor("myAnalogInput", HASensorNumber::PrecisionP1);

// You can also specify the precision of the sensor by providing the second argument to the constructor as follows:
// HASensorNumber analogSensor("myAnalogInput", HASensorNumber::PrecisionP2);
// HASensorNumber analogSensor("myAnalogInput", HASensorNumber::PrecisionP3);

void setup() {
    // you don't need to verify return status
    Ethernet.begin(mac);

    // set device's details (optional)
    device.setName("Arduino");
    device.setSoftwareVersion("1.0.0");

    // configure sensor (optional)
    analogSensor.setIcon("mdi:home");
    analogSensor.setName("Analog voltage");
    analogSensor.setUnitOfMeasurement("V");

    mqtt.begin(BROKER_ADDR);
}

void loop() {
    Ethernet.maintain();
    mqtt.loop();

    if ((millis() - lastUpdateAt) > 1000) { // 1000ms debounce time
        uint16_t reading = analogRead(ANALOG_PIN);
        float voltage = reading * 5.f / 1023.f; // 0.0V - 5.0V

        analogSensor.setValue(voltage);
        lastUpdateAt = millis();
    }
}
