#include <AUnit.h>
#include <ArduinoHATests.h>

#define prepareTest \
    PubSubClientMock* mock = new PubSubClientMock(); \
    HADevice device(testDeviceId); \
    HAMqtt mqtt(mock, device);

#define flushSerializer(mock, serializer) \
    assertTrue(serializer != nullptr); \
    mock->connectDummy(); \
    mock->beginPublish(testTopic, 0, false); \
    serializer->flush(); \
    mock->endPublish();

#define assertJson(mock, serializer, expectedJson) \
{ \
    assertStringCaseEqual(mock->getMessageTopic(), testTopic); \
    assertStringCaseEqual(mock->getMessageBuffer(), F(expectedJson)); \
    assertEqual((uint16_t)strlen_P(reinterpret_cast<const char *>(expectedJson)), serializer->calculateSize()); \
}

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testUniqueId = "uniqueSensor";
static const char* testTopic = "testTopic";

test(BinarySensorTest, invalid_unique_id) {
    prepareTest

    HABinarySensor sensor(nullptr, false);
    sensor.buildSerializer();
    HASerializer* serializer = sensor.getSerializer();

    assertTrue(serializer == nullptr);
}

test(BinarySensorTest, default_params) {
    prepareTest

    HABinarySensor sensor(testUniqueId, false);
    sensor.buildSerializer();
    HASerializer* serializer = sensor.getSerializer();

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"aha/testDevice/uniqueSensor/stat_t\"}");
}

test(BinarySensorTest, name_setter) {
    prepareTest

    HABinarySensor sensor(testUniqueId, false);
    sensor.setName("testName");
    sensor.buildSerializer();
    HASerializer* serializer = sensor.getSerializer();

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"name\":\"testName\",\"uniq_id\":\"uniqueSensor\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"aha/testDevice/uniqueSensor/stat_t\"}");
}

test(BinarySensorTest, device_class) {
    prepareTest

    HABinarySensor sensor(testUniqueId, false);
    sensor.setDeviceClass("testClass");
    sensor.buildSerializer();
    HASerializer* serializer = sensor.getSerializer();

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"uniq_id\":\"uniqueSensor\",\"dev_cla\":\"testClass\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"aha/testDevice/uniqueSensor/stat_t\"}");
}

test(BinarySensorTest, icon_setter) {
    prepareTest

    HABinarySensor sensor(testUniqueId, false);
    sensor.setIcon("testIcon");
    sensor.buildSerializer();
    HASerializer* serializer = sensor.getSerializer();

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"uniq_id\":\"uniqueSensor\",\"ic\":\"testIcon\",\"dev\":{\"ids\":\"testDevice\"},\"stat_t\":\"aha/testDevice/uniqueSensor/stat_t\"}");
}

test(BinarySensorTest, default_state_false) {
    prepareTest

    HABinarySensor sensor(testUniqueId, false);
    assertEqual(false, sensor.getState());
}

test(BinarySensorTest, default_state_true) {
    prepareTest

    HABinarySensor sensor(testUniqueId, true);
    assertEqual(true, sensor.getState());
}

test(BinarySensorTest, publish_state_on) {
    prepareTest

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId, false);
    sensor.setState(!sensor.getState());

    assertStringCaseEqual(mock->getMessageTopic(), F("aha/testDevice/uniqueSensor/stat_t"));
    assertStringCaseEqual(mock->getMessageBuffer(), F("ON"));
    assertEqual(mock->getMessageLength(), (size_t)2);
    assertTrue(mock->isMessageRetained());
}

test(BinarySensorTest, publish_state_off) {
    prepareTest

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId, true);
    sensor.setState(!sensor.getState());

    assertStringCaseEqual(mock->getMessageTopic(), F("aha/testDevice/uniqueSensor/stat_t"));
    assertStringCaseEqual(mock->getMessageBuffer(), F("OFF"));
    assertEqual(mock->getMessageLength(), (size_t)3);
}

test(BinarySensorTest, publish_state_debounce) {
    prepareTest

    mock->connectDummy();
    HABinarySensor sensor(testUniqueId, true); // initial state is true
    sensor.setState(true);

    // it shouldn't publish data if state doesn't change
    assertStringCaseEqual(mock->getMessageTopic(), "");
    assertStringCaseEqual(mock->getMessageBuffer(), "");
    assertEqual(mock->getMessageLength(), (size_t)0);
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
