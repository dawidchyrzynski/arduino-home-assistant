#include <AUnit.h>
#include <ArduinoHA.h>

#define assertConfig(mock, expectedJson) \
{ \
    mock->connectDummy(); \
    sensor.publishConfigTest(); \
    assertSingleMqttMessage(configTopic, expectedJson, true) \
    assertTrue(sensor.getSerializer() == nullptr); \
}

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueSensor";
static const char* configTopic = "homeassistant/binary_sensor/testDevice/uniqueSensor/config";
static const char* stateTopic = "testData/testDevice/uniqueSensor/stat_t";

test(BinarySensorTest, invalid_unique_id) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(nullptr, false);
    sensor.buildSerializerTest();
    HASerializer* serializer = sensor.getSerializer();

    assertTrue(serializer == nullptr);
}

test(BinarySensorTest, default_params) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId, false);
    assertConfig(mock, "{\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}");
}

test(BinarySensorTest, name_setter) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId, false);
    sensor.setName("testName");

    assertConfig(mock, "{\"name\":\"testName\",\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}");
}

test(BinarySensorTest, device_class) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId, false);
    sensor.setDeviceClass("testClass");

    assertConfig(mock, "{\"uniq_id\":\"uniqueSensor\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}");
}

test(BinarySensorTest, icon_setter) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId, false);
    sensor.setIcon("testIcon");

    assertConfig(mock, "{\"uniq_id\":\"uniqueSensor\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"testData/testDevice/uniqueSensor/stat_t\"}");
}

test(BinarySensorTest, default_state_false) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId, false);
    assertEqual(false, sensor.getState());
}

test(BinarySensorTest, default_state_true) {
    initMqttTest(testDeviceId)

    HABinarySensor sensor(testUniqueId, true);
    assertEqual(true, sensor.getState());
}

test(BinarySensorTest, publish_state_on) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId, false);
    sensor.setState(!sensor.getState());

    assertSingleMqttMessage(stateTopic, "ON", true)
}

test(BinarySensorTest, publish_state_off) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId, true);
    sensor.setState(!sensor.getState());

    assertSingleMqttMessage(stateTopic, "OFF", true)
}

test(BinarySensorTest, publish_state_debounce) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId, true); // initial state is true
    sensor.setState(true);

    // it shouldn't publish data if state doesn't change
    assertEqual(mock->getFlushedMessagesNb(), 0);
}

test(BinarySensorTest, publish_state_debounce_skip) {
    initMqttTest(testDeviceId)

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId, true); // initial state is true
    sensor.setState(true, true);

    assertSingleMqttMessage(stateTopic, "ON", true)
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
