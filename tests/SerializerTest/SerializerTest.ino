#include <AUnit.h>
#include <ArduinoHATests.h>

#define prepareTest \
    PubSubClientMock* mock = new PubSubClientMock(); \
    HADevice device(testDeviceId); \
    HAMqtt mqtt(mock, device); \
    HASerializer serializer(nullptr);

#define flushSerializer(mock, serializer) \
    mock->connectDummy(); \
    mock->beginPublish(testTopic, 0, false); \
    serializer.flush(); \
    mock->endPublish();

#define assertJson(mock, serializer, expectedJson) \
{ \
    const char* json = expectedJson; \
    assertStringCaseEqual(mock->getMessageTopic(), testTopic); \
    assertStringCaseEqual(mock->getMessageBuffer(), json); \
    assertEqual(strlen(json), serializer.calculateSize()); \
}

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testTopic = "testTopic";

test(SerializerTest, empty_json) {
    prepareTest
    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{}");
}

test(SerializerTest, char_field) {
    prepareTest

    serializer.set(HANameProperty, "XYZ");

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"name\":\"XYZ\"}");
}

test(SerializerTest, char_fields) {
    prepareTest

    serializer.set(HADeviceClassProperty, "Class");
    serializer.set(HAIconProperty, "Icon");

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"dev_cla\":\"Class\",\"ic\":\"Icon\"}");
}

test(SerializerTest, field_override) {
    prepareTest

    serializer.set(HADeviceClassProperty, "Class1");
    serializer.set(HADeviceClassProperty, "Class2");

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"dev_cla\":\"Class2\"}");
}

test(SerializerTest, skip_null_fields) {
    prepareTest

    serializer.set(HADeviceClassProperty, "Class");
    serializer.set(HAIconProperty, nullptr);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"dev_cla\":\"Class\"}");
}

test(SerializerTest, bool_false_field) {
    prepareTest

    bool value = false;
    serializer.set(HANameProperty, &value, HASerializer::BoolPropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"name\":false}");
}

test(SerializerTest, bool_true_field) {
    prepareTest

    bool value = true;
    serializer.set(HANameProperty, &value, HASerializer::BoolPropertyType);

    flushSerializer(mock, serializer);
    assertJson(mock, serializer, "{\"name\":true}");
}

// to do: float
// to do: unsigned number
// to do: signed number
// to do: zero number
// to do: array
// to do: topic
// to do: two topics
// to do: topics duplicate call
// to do: with device
// to do: with availability

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
