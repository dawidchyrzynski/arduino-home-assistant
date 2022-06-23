#include <AUnit.h>
#include <ArduinoHA.h>

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueSensor";
static const char* configTopic = "homeassistant/sensor/testDevice/uniqueSensor/config";
static const char* stateTopic = "testData/testDevice/uniqueSensor/stat_t";

test(SensorTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HASensor sensor(nullptr);
    sensor.buildSerializerTest();
    HASerializer* serializer = sensor.getSerializer();

    assertTrue(serializer == nullptr);
}

test(SensorTest, default_params) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, availability) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setAvailability(true);
    mqtt.loop();

    // availability is published after config in HASensor
    assertMqttMessage(
        1,
        "testData/testDevice/uniqueSensor/avty_t",
        "online",
        true
    )
}

test(SensorTest, name_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setName("testName");

    assertEntityConfig(
        mock,
        sensor,
        "{\"name\":\"testName\",\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, device_class_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setDeviceClass("testClass");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, force_update_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setForceUpdate(true);

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"frc_upd\":true,\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, icon_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setIcon("testIcon");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, unit_of_measurement_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setUnitOfMeasurement("%");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"unit_of_meas\":\"%\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, value_template_setter) {
    initMqttTest(testDeviceId)

    HASensor sensor(testUniqueId);
    sensor.setValueTemplate("abc");

    assertEntityConfig(
        mock,
        sensor,
        "{\"uniq_id\":\"uniqueSensor\",\"val_tpl\":\"abc\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}"
    )
}

test(SensorTest, publish_value) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HASensor sensor(testUniqueId);
    bool result = sensor.setValue("test123");

    assertSingleMqttMessage(stateTopic, "test123", true)
    assertTrue(result);
}

void setup()
{
    Serial.begin(115200);
    while (!Serial);
}

void loop()
{
    TestRunner::run();
    delay(1);
}
