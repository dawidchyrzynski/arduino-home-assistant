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
    mock->endPublish(); \

#define assertJson(mock, expectedJson) \
    assertStringCaseEqual(mock->getMessageTopic(), testTopic); \
    assertStringCaseEqual(mock->getMessageBuffer(), expectedJson);

using aunit::TestRunner;

static const char* testDeviceId = "testDevice";
static const char* testTopic = "testTopic";

test(SerializerTest, empty_json) {
    prepareTest
    flushSerializer(mock, serializer);

    assertStringCaseEqual(mock->getMessageTopic(), testTopic);
    assertStringCaseEqual(mock->getMessageBuffer(), "{}");
}

test(SerializerTest, char_fields) {
    prepareTest

    serializer.set(HADeviceClassProperty, "Class");
    serializer.set(HAIconProperty, "Icon");

    flushSerializer(mock, serializer);
    assertJson(mock, "{\"dev_cla\":\"Class\",\"ic\":\"Icon\"}");
}

test(SerializerTest, field_override) {
    prepareTest

    serializer.set(HADeviceClassProperty, "Class1");
    serializer.set(HADeviceClassProperty, "Class2");

    flushSerializer(mock, serializer);
    assertJson(mock, "{\"dev_cla\":\"Class2\"}");
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
